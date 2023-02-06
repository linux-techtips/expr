#![allow(dead_code)]
#![allow(unused_imports)]
#![allow(unused_variables)]

#[macro_use]
mod ast;
mod codegen;
mod parser;

use parser::*;

#[inline]
fn make_static(s: String) -> &'static str {
    Box::leak(s.into_boxed_str())
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let program = make_static(std::fs::read_to_string("test.expr")?);
    //let program = include_str!("../test.expr");
    let (input, value) = Parser::parse_module(program)?;

    println!("{value:#?} : {input}");

    Ok(())
}

