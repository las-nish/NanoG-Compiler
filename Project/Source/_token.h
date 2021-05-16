/// token header
/// @file : _token.h
/// @author : lasan nishshanka (@las_nish)
/// @date 2021-05-13 & 12:16:19
/// @copyright Copyright (c) 2020-2021 lasan nishshanka

#ifndef _HEADER_TOKEN_
#define _HEADER_TOKEN_

enum {
  o_plus,
  o_minus,
  o_multi,
  o_divis,
  o_modulus,
  o_incr,
  o_decr,
  o_eqto,
  o_noteq,
  o_les,
  o_great,
  o_leseq,
  o_greateq,
  o_and,
  o_or,
  o_not,
  o_bitand,
  o_inor,
  o_xor,
  o_unary,
  o_lshift,
  o_rshift,
  o_eq,
  o_pluseq,
  o_minuseq,
  o_multieq,
  o_diviseq,
  o_moduluseq,
  o_lshifteq,
  o_rshifteq,
  o_bitandeq,
  o_xoreq,
  o_inoreq,

  k_size,

  k_asm,
  k_break,
  k_char,
  k_else,
  k_empty,
  k_fn,
  k_goto,
  k_if,
  k_int,
  k_import,
  k_link,
  k_place,
  k_return,
  k_string,
  k_while,
  k_zero,

  s_lprn,
  s_rprn,
  s_lcbrk,
  s_rcbrk,
  s_lbrk,
  s_rbrk,
  s_col,
  s_scol,
  s_dot,
  s_com,
  s_arow,
  s_dolar,

  t_id,
  t_int,
  t_char,
  t_string,
  t_char_string,

  t_eof
};

#endif