"""
Lexical analysis for the language C
"""
from collections import OrderedDict, namedtuple
import re

Token = namedtuple('Token', ['typ', 'value', 'line', 'column'])

def tokenize(s):
    #задаємо множину ключових слів мови С
    keywords = {'auto', 'break', 'case', 'char', 'const', 'continue', 'Default',
                'do', 'double', 'else', 'enum', 'extern', 'new', 'delete', 'float', 'for',' goto',
                'if', 'int', 'long', 'register', 'return', 'short', 'signed', 'sizeof',
                'static', 'struct', 'switch', 'typedef', 'union', 'unsigned', 'void',
                'volatile', 'while', 'int86'}
    keyw_hash = dict.fromkeys(keywords)
    #Order is very important in our case
    token_specification = OrderedDict([
        ('COMMENT', r'((//[^\n]*)|(/\*.*\*/))'),
        ('DIRECTIVE', r'(#[^\n]*)'),
        ('ID',      r'([A-Za-z_][A-Za-z0-9_]*)'),
        ('HEX',     r'(0[xX][0-9A-Fa-f]+)'),
        ('FLOAT',   r'(\-?\d+\.\d*)'),
        ('INT',     r'(\d+)'),
        ('BRACKET', r'([][(){}])'),
        ('PUNCTUATION', r'([?,;:])'),
        ('OP',      r'((\->)|(\-\-)|(\+\+)|&&|(\|\|)|<<=|>>=|<<|>>|([<>!+\-*/^%&|=]=)|[!~^|<>=+*\/\-\.&%])'),
        ('NEWLINE', r'(\n)'),
        ('SKIP',    r'([ \t])'),
        ('STRING',  r'(".*?")'),
        ('CHAR',    r"('.?')")
    ])
    
    tok_regex = '|'.join('(?P<%s>%s)' % (key, value) for (key, value) in token_specification.items())
    get_token = re.compile(tok_regex,re.DOTALL).match
    line = 1#
    pos = line_start = 0
    mo = get_token(s)
    tokens_hash = OrderedDict()
    num = 0;
    while pos != len(s):
        if mo is not None:
            typ = mo.lastgroup
            if typ == 'NEWLINE':
                line_start = pos
                line += 1
            elif typ != 'SKIP':
                val = mo.group(typ)
                if typ == 'ID' and val in keyw_hash:
                    typ = 'KEYWORD'
                tokens_hash[num] = Token(typ, val, line, mo.start()-line_start)
            pos = mo.end()
        else:
            tokens_hash[num] = Token('UNEXPECTED CHARACTER', s[pos], line, pos - line_start)
            pos = pos + 1
        mo = get_token(s, pos)
        num = num + 1
    return tokens_hash

statements = '''
/*d
*/
~
`@
''
гн
#
@
(
3.5
3
a.b
a = b
a + b
a - b
+a
-a
a*b
a/b
a%b
++a
a++
--a
a--
a==b
a!=b
a>b
a<b
a>=b
a<=b
!a
a&&b
a||b
~a
a&b
a|b
a^b
a<<b
a>>b
a+=b
a-=b
a*=b
a/=b
a%=b
a&=b
a|=b
a^=b
a<<=b
a>>=b
a[b]
a{b}
a?b:c
a.b
a,b;
a:b
a->b
#using
0X18
""

if keywordTrue then do for all r, in  reg_5t r 3.5 3. <<f>>  "ff" 'a'
fff//commenteeee
fff
'''

#statements = open(r'E:\Program Files\Python Programs\InputLab3.txt','r').read()#
out = open(r'E:\Program Files\Python Programs\OutPutLab3.txt','w')
tokens_hash = tokenize(statements)
for token in tokens_hash.values():
    #if token.typ != 'UNEXPECTED CHARACTER':
        out.write(token.value.ljust(50) + ' - \t' + token.typ.ljust(15) + ' (line = ' + str(token.line) + ', column = ' + str(token.column) + ')' + '\n')
        out.flush()
   # else:
        #out.close()
        #raise RuntimeError('Unexpected character %r on line %d' %(token.value, token.line))
out.close()
