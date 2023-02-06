use std::{borrow::Cow, cell::Cell};

#[derive(Debug)]
pub enum Operator {
    Add,
    Sub,
    Mul,
    Div,
}

#[derive(Debug)]
pub enum Constness {
    Const,
    Mut,
}

pub type NodeRef<'n> = Box<Node<'n>>;

#[derive(Debug)]
pub enum ExprType<'t> {
    Fn(NodeRef<'t>),
    UserDefined(&'t str),
    S64,
    U64,
    D64,
    Bool,
}

#[derive(Debug)]
pub enum Node<'n> {
    Module(Vec<Node<'n>>),
    FnBlockExpr(Vec<Node<'n>>),
    StructBlockExpr(Vec<Node<'n>>),
    EnumBlockExpr(Vec<Node<'n>>),
    StmtExpr {
        constness: Constness,
        name: NodeRef<'n>,
        ty: Option<NodeRef<'n>>,
        expr: NodeRef<'n>,
    },
    BinExpr {
        op: Operator,
        lhs: NodeRef<'n>,
        rhs: NodeRef<'n>,
    },
    UnExpr {
        op: Operator,
        child: NodeRef<'n>,
    },
    FnDecl {
        args: Vec<Node<'n>>,
        ret: Option<NodeRef<'n>>,
    },
    FnParam {
        name: NodeRef<'n>,
        ty: NodeRef<'n>,
    },
    FnCall {
        name: NodeRef<'n>,
        args: Vec<Node<'n>>,
    },
    Type(ExprType<'n>),
    LitStr(Cow<'n, str>),
    LitChr(char),
    LitDec(f64),
    LitInt(i64),
    LitBool(bool),
    Ident(&'n str),
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let ty = Node::FnDecl {
            args: vec![],
            ret: None,
        };

        let print = Node::FnCall {
            name: Box::new(Node::Ident("println")),
            args: vec![
                Node::LitStr(Cow::Borrowed("Hellol {}")),
                Node::LitStr(Cow::Borrowed("Asher")),
            ],
        };

        let statements = Node::BlockExpr(vec![print]);

        let entry_fn = Node::StmtExpr {
            constness: Constness::Const,
            name: Box::new(Node::Ident("entry")),
            ty: Some(Box::new(ty)),
            expr: Box::new(statements),
        };

        let module = Node::Module(vec![entry_fn]);

        println!("{module:#?}");
    }
}
