use crate::ast::*;
use crate::codegen::*;

use std::cell::Cell;

pub use nom::{
    branch::alt,
    bytes::complete::{escaped, is_not, tag, take, take_till, take_till1, take_while1},
    character::{
        complete::{alphanumeric1, anychar, char, digit1, multispace0, none_of, satisfy},
        is_alphabetic, is_hex_digit,
    },
    combinator::{all_consuming, map, map_opt, map_res, opt, recognize, value, verify},
    error::{Error, ParseError},
    multi::{fold_many_m_n, many0, many0_count, many1, separated_list0},
    sequence::{delimited, pair, preceded, separated_pair, terminated, tuple},
    IResult,
};

use std::borrow::Cow;

macro_rules! parse_bin {
    ($name:ident, $fn:path, {$($arms:tt)*}) => {
        pub fn $name(input: &'p str) -> IResult<&'p str, Node> {
            let (mut input, mut lhs) =
                delimited(multispace0, $fn, multispace0)(input)?;
            loop {
                let op = match input.chars().nth(0) {
                    $($arms)*
                    _ => return Ok((input, lhs.into())),
                };

                let (tail, rhs) = delimited(multispace0, $fn, multispace0)(&input[1..])?;

                lhs = Node::BinExpr {
                    op,
                    lhs: lhs.into(),
                    rhs: rhs.into(),
                };

                input = tail
            }
        }
    };
}

#[derive(Debug)]
pub(crate) enum StrFragment<'f> {
    Unescaped(&'f str),
    Escaped(char),
}

pub(crate) struct Parser<'p> {
    pub fname: &'p str,
    pub codegen: CodeGen<'p>,
}

impl<'p> Parser<'p> {
    pub fn parse_module(input: &'p str) -> IResult<&'p str, Node> {
        map(
            many1(delimited(multispace0, Self::parse_toplevel, multispace0)),
            Node::Module,
        )(input)
    }

    pub fn parse_toplevel(input: &'p str) -> IResult<&'p str, Node> {
        Self::parse_stmt_expr(input)
    }

    pub(crate) fn parse_fn_block(input: &'p str) -> IResult<&'p str, Node> {
        map(
            delimited(
                char('{'),
                many0(delimited(multispace0, Self::parse_stmt_expr, multispace0)),
                char('}'),
            ),
            Node::FnBlockExpr,
        )(input)
    }

    parse_bin!(
        parse_bin_multiplicative,
        Self::parse_primary,
        {
            Some('*') => Operator::Mul,
            Some('/') => Operator::Div,
        }
    );
    parse_bin!(
        parse_bin_addative,
        Self::parse_bin_multiplicative,
        {
            Some('+') => Operator::Add,
            Some('-') => Operator::Sub,
        }
    );

    pub(crate) fn parse_stmt_expr(input: &'p str) -> IResult<&'p str, Node> {
        map(
            tuple((
                terminated(terminated(Self::parse_ident, multispace0), tag(":")),
                opt(delimited(multispace0, Self::parse_type, multispace0)),
                terminated(
                    map_opt(take(1usize), |s| match s {
                        ":" => Some(Constness::Const),
                        "=" => Some(Constness::Mut),
                        _ => None,
                    }),
                    multispace0,
                ),
                Self::parse_expr,
            )),
            |(name, ty, constness, expr)| Node::StmtExpr {
                constness,
                name: Box::new(name),
                ty: ty.map(|t| t.into()),
                expr: Box::new(expr),
            },
        )(input)
    }

    pub(crate) fn parse_paren_expr(input: &'p str) -> IResult<&'p str, Node> {
        map(delimited(char('('), Self::parse_expr, char(')')), |node| {
            node
        })(input)
    }

    pub(crate) fn parse_expr(input: &'p str) -> IResult<&'p str, Node> {
        alt((Self::parse_bin_addative, Self::parse_fn_block))(input)
    }

    pub(crate) fn parse_primary(input: &'p str) -> IResult<&'p str, Node> {
        alt((Self::parse_paren_expr, Self::parse_ident, Self::parse_lit))(input)
    }

    pub(crate) fn parse_param(input: &'p str) -> IResult<&'p str, Node> {
        map(
            separated_pair(
                Self::parse_ident,
                separated_pair(multispace0, tag(":"), multispace0),
                Self::parse_type,
            ),
            |(name, ty)| Node::FnParam {
                name: name.into(),
                ty: ty.into(),
            },
        )(input)
    }

    pub(crate) fn parse_type(input: &'p str) -> IResult<&'p str, Node> {
        alt((
            map(Self::parse_fn_ty, |node| {
                Node::Type(ExprType::Fn(Box::new(node)))
            }),
            map(Self::parse_ident_raw, |s: &str| match s {
                "s64" => Node::Type(ExprType::S64),
                "u64" => Node::Type(ExprType::U64),
                "d64" => Node::Type(ExprType::D64),
                _ => Node::Type(ExprType::UserDefined(s)),
            }),
        ))(input)
    }

    pub(crate) fn parse_fn_ty(input: &'p str) -> IResult<&'p str, Node> {
        map(
            pair(
                preceded(
                    terminated(tag("fn"), multispace0),
                    delimited(
                        terminated(char('('), multispace0),
                        separated_list0(
                            separated_pair(multispace0, tag(","), multispace0),
                            Self::parse_param,
                        ),
                        char(')'),
                    ),
                ),
                opt(preceded(
                    separated_pair(multispace0, tag("->"), multispace0),
                    Self::parse_type,
                )),
            ),
            |(args, ret_opt)| Node::FnDecl {
                args,
                ret: ret_opt.map(|ret| ret.into()),
            },
        )(input)
    }

    pub(crate) fn parse_lit(input: &'p str) -> IResult<&'p str, Node> {
        alt((
            Self::parse_lit_str,
            Self::parse_lit_chr,
            Self::parse_lit_dec,
            Self::parse_lit_int,
        ))(input)
    }

    pub(crate) fn parse_lit_str(input: &'p str) -> IResult<&'p str, Node> {
        let mut result = Cow::Borrowed("");

        let parse_double_quote = char('"');
        let (mut input, _) = parse_double_quote(input)?;

        loop {
            let dq_err = match parse_double_quote(input) {
                Ok((tail, _)) => {
                    return Ok((tail, Node::LitStr(result)));
                }
                Err(nom::Err::Error(err)) => err,
                Err(err) => return Err(err),
            };

            let tail = match Parser::parse_str_fragment(input) {
                Ok((tail, StrFragment::Escaped(c))) => {
                    result.to_mut().push(c);
                    tail
                }
                Ok((tail, StrFragment::Unescaped(s))) => {
                    if result.is_empty() {
                        result = Cow::Borrowed(s);
                    } else {
                        result.to_mut().push_str(s);
                    }
                    tail
                }
                Err(nom::Err::Error(err)) => return Err(nom::Err::Error(dq_err.or(err))),
                Err(err) => return Err(err),
            };

            input = tail
        }
    }

    pub(crate) fn parse_lit_chr(input: &'p str) -> IResult<&'p str, Node> {
        map(
            delimited(
                char('\''),
                alt((Self::parse_escaped_chr, Self::parse_single_unescaped_chr)),
                char('\''),
            ),
            Node::LitChr,
        )(input)
    }

    pub(crate) fn parse_lit_dec(input: &'p str) -> IResult<&'p str, Node> {
        map_res(
            recognize(tuple((
                opt(char('-')),
                digit1,
                opt(tuple((char('.'), digit1))),
                opt(tuple((
                    alt((char('e'), char('E'))),
                    opt(alt((char('+'), char('-')))),
                ))),
                digit1,
            ))),
            |s: &'p str| s.parse().map(Node::LitDec),
        )(input)
    }

    pub(crate) fn parse_lit_int(input: &'p str) -> IResult<&'p str, Node> {
        map_res(recognize(tuple((opt(char('-')), digit1))), |s: &'p str| {
            s.parse().map(Node::LitInt)
        })(input)
    }

    pub(crate) fn parse_bool(input: &'p str) -> IResult<&'p str, bool> {
        alt((value(true, tag("true")), value(false, tag("false"))))(input)
    }

    pub(crate) fn parse_ident(input: &'p str) -> IResult<&'p str, Node> {
        map(Self::parse_ident_raw, Node::Ident)(input)
    }

    pub(crate) fn parse_ident_raw(input: &'p str) -> IResult<&'p str, &'p str> {
        map(
            recognize(pair(
                verify(anychar, |&c| c.is_alphabetic()),
                many0_count(preceded(opt(char('_')), alphanumeric1)),
            )),
            |s: &str| s,
        )(input)
    }

    pub(crate) fn parse_single_unescaped_chr(input: &'p str) -> IResult<&'p str, char> {
        map_opt(take(1usize), |s: &str| match s.chars().next() {
            Some('\'') => None,
            Some(c) => Some(c),
            _ => None,
        })(input)
    }

    // TODO: support runes natively
    fn parse_escaped_unicode(input: &'p str) -> IResult<&'p str, char> {
        preceded(
            char('u'),
            map_opt(
                map(
                    recognize(fold_many_m_n(
                        4,
                        4,
                        satisfy(|c| is_hex_digit(c as u8)),
                        || {},
                        |(), _c| (),
                    )),
                    |s| u32::from_str_radix(s, 16).unwrap(),
                ),
                char::from_u32,
            ),
        )(input)
    }

    fn parse_escaped_chr(input: &'p str) -> IResult<&'p str, char> {
        preceded(
            char('\\'),
            alt((
                value('/', char('/')),
                value('"', char('"')),
                value('\'', char('\'')),
                value('\\', char('\\')),
                value('\n', char('n')),
                value('\r', char('r')),
                value('\t', char('t')),
                Parser::parse_escaped_unicode, // TODO: support runes natively
            )),
        )(input)
    }

    fn parse_str_fragment(input: &'p str) -> IResult<&'p str, StrFragment<'p>> {
        alt((
            map(Parser::parse_escaped_chr, StrFragment::Escaped),
            map(is_not("\"\\"), StrFragment::Unescaped),
        ))(input)
    }
}
