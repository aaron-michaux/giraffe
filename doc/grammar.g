
// Note: all imports must appear before declaraions, #include aside.
TranslationUnit:
   | PreprocCommand*
   | 'export' 'module' ModuleName ';'
   | 'export'? 'import' ModuleName ';'
   ;

ModuleName:
   | IDENTIFIER (':' IDENTIFIER)?
   | PathSpec
   ;

PreprocCommand:
   | '#define' IDENTIFIER token-string?
   | '#define' IDENTIFIER (IDENTIFIER?, ..., IDENTIFIER?) token-string?
   | '#include' PathSpec
   | '#line' DIGITS ('"' PATHSPEC '"')?
   | '#undef' IDENTIFIER
   | '#error' token-string
   | '#pragma' token-string
   | if-part elif-part* else-part? '#endif'
   ;

PathSpec:
   | '"' PATHSPEC '"'
   | '<' PATHSPEC '>'
   ;

if-part:
   | '#if' constant-expression
   | '#ifdef' IDENTIFIER
   | '#ifndef' IDENTIFIER
   ;
      
elif-part:
   | '#elif' constant-expression
   ;

else-part:
   | '#else' text
   ;      

ConstantExpression:
   | 'defined' '(' IDENTIFIER ')'
   | 'defined'  IDENTIFIER
   | Expression
   ;

// Note: uses precidence climbing on operators
Expression:
   | UnaryExpression
   | UnaryExpression BinaryOperator Expression
   ;

UnaryExpression:
   | UnaryOperator PrimaryExpression
   | PrimaryExpression
   ;

BinaryExpression:
      

PrimaryExpression:
   | IDENTIFIER
   | INTEGER
   | HEX_INTEGER
   | '(' Expression ')'
   ;

UnaryOperator: '+' | '-' | '!' | '~' ;

BinaryOperator:
   | '%' | '*' | '/' | '+' | '-'
   | '>' | '<' | '<=' | '>=' | '==' | '!='
   | '&&' | '||'
   | '|' | '^' | '&' | '<<' | '>>'
   ;
      
