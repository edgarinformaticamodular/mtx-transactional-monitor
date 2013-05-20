#ifndef ECC_TO_SQL_SAFE_HPP
#define ECC_TO_SQL_SAFE_HPP

namespace ecc {

  class ToSqlSafe {
  public:
	char operator()(char a_char);
  };
}//namespace

#endif
