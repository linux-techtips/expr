use inkwell::{
    basic_block::BasicBlock,
    builder::Builder,
    context::Context,
    module::{Linkage, Module},
    targets::{CodeModel, FileType, InitializationConfig, RelocMode, Target, TargetMachine},
    types::IntType,
    values::{FunctionValue, PointerValue},
    {AddressSpace, IntPredicate, OptimizationLevel},
};


pub struct CodeGen<'ctx> {
    pub context: &'ctx Context,
    pub module: Module<'ctx>,
    pub builder: Builder<'ctx>,
}
