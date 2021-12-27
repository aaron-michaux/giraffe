
// Note: all imports must appear before declaraions, #include aside.
TranslationUnit:
   | PreprocCommand*
   | 'export'? ('import' | 'module') ModuleName ';'
   ;

ModuleName:
   | IDENTIFIER (':' IDENTIFIER)?
   | PathSpec
   ;

PreprocCommand:
   | '#define' IDENTIFIER token-string?
   | '#define' IDENTIFIER (IDENTIFIER?, ..., IDENTIFIER?) token-string?
   | '#include' PathSpec
   | '#undef' IDENTIFIER
   | '#error' token-string
   | IfPart ElifPart* ElsePart? '#endif'
   ;

PathSpec:
   | '"' PATHSPEC '"'
   | '<' PATHSPEC '>'
   ;

IfPart:
   | '#if' ConstantExpression
   | '#ifdef' IDENTIFIER
   | '#ifndef' IDENTIFIER
   ;
      
ElifPart:
   | '#elif' ConstantExpression
   ;

ElsePart:
   | '#else' text
   ;      

ConstantExpression:
   | Expression
   ;

// Note: uses precidence climbing on operators
Expression: UnaryExpression (BinaryOperator Expression)? ;

UnaryExpression: UnaryOperator? PrimaryExpression ;

PrimaryExpressiion: IDENTIFIER | INTEGER | '(' Expression ')'
   ;

UnaryOperator: '+' | '-' | '!' | '~' | DEFINED;

BinaryOperator:
   | '%' | '*' | '/' | '+' | '-'
   | '>' | '<' | '<=' | '>=' | '==' | '!='
   | '&&' | '||'
   | '|' | '^' | '&' | '<<' | '>>'
   ;
      
