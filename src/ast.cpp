#include "include/ast.hpp"
#include "exl/reflection.hpp"

namespace Ast {

auto dump(const Node *node, const Depth depth) -> void {
  if (node == nullptr) {
    return;
  }
  match (*node)([=](auto &&arg) { eprintln("Unexpected node to dump"); },
                [=](IsNode auto &n) {
                  eprintln("made it");
                  println("Got a valid node, {}", *n.tok);
                },

                [=](NodeDecl n) {
                  println("{}NodeDecl [", depth);
                  
                  print("{} mutability: ", depth);
                  if (n.mut == Mut) {
                    println("mut");
                  }
                  println("const");
                
                  println("{} ident: ", depth);
                  dump(n.ident, Depth::increase(depth));

                  println("{} type: ", depth);
                  dump(n.type, Depth::increase(depth));

                  println("{} expr: ", depth);
                  dump(n.expr, Depth::increase(depth));

                  println("{}]", depth);
                },

                [=](NodeFnProto n) { // NOLINT
                  println("{}NodeFnProto [", depth);
                  println("{} return type: ", depth);
                  dump(n.ret_type, Depth::increase(depth));

                  println("{} params: ", depth);

                  for (auto &p : n.params) {
                    dump(p, Depth::increase(depth));
                  }

                  println("{}]", depth);
                },

                [=](NodeParamDecl n) {
                  println("{}NodeParamDecl [", depth);
                  println("{} ident: ", depth);
                  dump(n.ident, Depth::increase(depth));
                  println("{} type: ", depth);
                  dump(n.type, Depth::increase(depth));
                  println("{}]", depth);
                },

                [=](NodeBinExpr n) {
                  println("{}NodeBinExpr [", depth);

                  auto op = char{};
                  switch (n.op) {
                  default: {
                    eprintln("Invalid binary operand token, got {}", n.op);
                    std::abort();
                  }
                  case Tok::Plus: {
                    op = '+';
                    break;
                  }
                  case Tok::Dash: {
                    op = '-';
                    break;
                  }
                  case Tok::Asterix: {
                    op = '*';
                    break;
                  }
                  case Tok::ForwardSlash: {
                    op = '/';
                    break;
                  }
                  }

                  println("{} op: {}", depth, op);
                  println("{} lhs:", depth);
                  dump(n.lhs, Depth::increase(depth));
                  println("{} rhs:", depth);
                  dump(n.rhs, Depth::increase(depth));
                  println("{}]", depth);
                },
                [=](NodeLitInt n) {
                  println("{}NodeLitInt [", depth);
                  println("{} val: {}", depth, n.val);
                  println("{}]", depth);
                },
                [=](NodeLitDec n) {
                  println("{}NodeLitDec [", depth);
                  println("{} val: {}", depth, n.val);
                  println("{}]", depth);
                },
                [=](NodeLitChr n) {
                  println("{}NodeLitChr [", depth);
                  println("{} val: {}", depth, n.val);
                  println("{}]", depth);
                },
                [=](NodeLitStr n) {
                  println("{}NodeLitStr [", depth);
                  println("{} val: {}", depth, n.val);
                  println("{}]", depth);
                },
                [=](NodeIdent n) {
                  println("{}NodeIdent [", depth);
                  println("{} name: {}", depth, n.name);
                  println("{}]", depth);
                });
}

} // namespace Ast
