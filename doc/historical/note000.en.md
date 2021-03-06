> This is translation of historical paper [note000.txt](note000.txt) from Russian.

The goal of the lab. Write a prototype of the Refal compiler → C++. The task is
only to examine the compilation of functions on Refal into C++ code. In this
case, such tasks as the provision of advanced modularity, the conceptual
integrity of the language and others are not set. It is enough to provide
modularity (layout model) of C language (all the function names must be
declared before the first use, functions from other layout units must be
declared as extern, the local functions correspond to static functions of C
language, global ones to non-static ones). It is sufficient for the language to
support three kinds of atoms: ascii-characters, macrodigits (overflow is not
checked for simplicity), function names. Because all functions at first must be
declared, function names that are used only as flags must be declared as empty
(as in Refal-2). However, since the names correspond functions, let the
same-named local functions of different translation units by comparison in
equality (use of the same-named variables) are unequal to each other (in
contrast to Refal-2 and Refal-7, in which the equality of two names is defined
by their names). There are no built-in functions, all functions that are used
in the program either declared in the current translation unit or appear to be
external. Such useful extensions as abstract data types are not implied.
Existing subset of Refal is the basic Refal.

Since, despite the limitations, the language will be sufficiently advanced,
there will be development of the self-hosted compiler. Therefore, at first the
language will be implemented as a preprocessor that translates this Refal (let
us call it Simple Refal) to Module Refal, which will be written, of course, at
Module Refal.

In the future, in case of successful development, the Module Refal itself can
use this language as a back-end for getting ready-made exe-files.

# Results.

## \[1] The following programs were developed during the lab.

* srprep – is a preprocessor that translates the code on the Simple Refal
  into code on the Module Refal. Written on Module Refal. The problem of
  Simple and Module Refal modulation mismatch was solved as well as in the
  program Refal5-to-MRefal converter, used for the conversion of Module Refal
  sources from Refal-5 to Module Refal. Namely, we used a specific type of
  comments before the directives $EXTERN. However, unlike the already mentioned
  program Refal5-to-MRefal converter, the preprocessor performs a lexemic not
  line-by-line preprocessing. As in the case of the Refal 5 → Module Refal,
  the same comments were used for use in make-utility (see below).

  The preprocessor, in contrast to a full-fledged compiler, tested only narrow
  class of errors: required that the description of the name preceded its use. A
  fuller inspection was not performed, since Module Refal itself made further
  verification. In order to make reports about syntactic errors, issued by the
  compiler, more adequate, tokens preprocessor in the output file located errors
  on the same lines as in the original file.

* srmake – is a program that facilitates the assembly of projects on the Simple
  Refal. This program for simplicity did not check the time of the last change
  in the input and output files. As it was already mentioned, the program used
  special comments ('// FROM FileName') to search for dependent files.

* LexGen – is a lexical analyzer generator. At the entrance the program
  accepted description of the finite-state transducer (Mealy machine, since the
  actions were performed at transition in a new condition), on output there was
  created the finite state machine.

The syntax of the lexical analyzer is the following:

    Description = Element* .
    Element = SetDescr | Sentence .

    SetDescr = SETNAME '=' Set* '.' .
    Set = LITERAL | SETNAME .

    Sentence = NAME '=' Alternative { '|' Alternative } '.' .
    Alternative = [Set] [Flush] [NAME] .
    Flush = '!-' | NAMEDFLUSH | ERRORFLUSH .

    SETNAME = ':' lowercase-or-capital-letter-or-digit* ':' .
    LITERAL = "'" sequence of characters-from-the escape-sequences'"' .
    NAME = Refal identifier.
    NAMEDFLUSH = '!' Refal identifier.
    ERRORFLUSH = '!"' message string '"' .

The model of the finite-state transducer calculations is as follows. The
finite-state transducer can be in one of several states. In addition, the
transducer contains a character buffer, at the end of which it can add a symbol,
drop its contents into the output stream, or empty it. Being in some state the
finite-state transducer reads or does not read the next character from the
input stream, if it reads, it necessarily adds to the end of the buffer,
performs an action on the buffer (the buffer can be emptied with output or
non-output of content to the output stream) and goes into another state.

In some cases, the transducer may wait for the end of the input.

The program for a finite state machine consists of a set of sentences and
descriptions of sets.

The sentence contains the name of the current state to the left of the equal
sign, and a set of alternatives to the left, which makes it look similar to
BNF. An alternative consists of a number of input characters that can be read,
actions with a buffer, and a new state. Each of the components of the
alternative may be absent.

If the set of characters is specified and the current character exists (which
means it is not the end of input) falls into this set, then this alternative is
performed and the symbol is read.

If the set is not present, but the next state is present, then this alternative
is also activated, and the input symbol is not read – the input stream does not
change. When the alternative is activated, if the character is read, it is
added to the end of the buffer. The action can be absent (in this case nothing
happens with the buffer), it can be an operation of an unnamed reset (in this
case, the buffer is emptied), it can be a named reset (in this case, the
content is discarded with this name – the format of the expression `(s.Name e.
Content)`, where `s.Name` is the identifier – the name of the reset (in the case
of Simple Refal – the name of the function), `e.Content` is the contents of the
buffer at the moment, including the read symbol. If the action is a reset-error
message, then in the output TokenError-token is output.

Alternatives are checked in turn one by one. If none of the alternatives work,
then default handlers are triggered. If there was no input end, then an
unexpected `TokenUnexpected` character is thrown, before it the contents of the
buffer as  `TokenAccum`, if the end of the file is found, then
`TokenUnexpectedEOF` is thrown instead of TokenUnexpected.

The set at the beginning of the alternative can be specified in two ways – as
a set of explicitly listed characters, and as a named set. The declaration of
a named set consists of the name of the set, the equal sign and enumeration of
the sets included in it. The declaration of a set, like the sentence of an
automaton, ends with a period. The content of the set is the union of the sets
listed to the right of the sign equals. Therefore recursive dependence of sets
is admissible. Named sets allow not only reducing the description of
sentences, but also to increase the clarity of the program.

Also there is a special named set `:Any:`, which includes all possible symbols.

By convention, the machine starts its work from the `Root` state with an empty
buffer.

The description of the automate is located inside the commentary, which must
start with a new line with the string `/*GEN:TOKENS` and end with a line (also
in a separate line) `GEN:END*/`. The machine code is generated immediately
after this comment and to the end of the file. If there is any text between the
last line of the comment and the end of the file, then this text is deleted.

Despite some primitiveness of both the syntax and the semantics of automata,
that was enough to recognize such complex constructs as C/C ++ comments and
strings with escape sequences. The disadvantages are that the absence of any
member of the alternative leads to the complexities of both the parser code of
the machine and the actual description of the vocabulary. To make it noticeable
the presence/absence of the elements of alternatives, you must explicitly
format the text in the form of a table (which I personally do not like to do).
It would be more convenient if special symbols or keywords were used instead of
the elements.

Another disadvantage is that the generated code is quite large, because the
result of the generation are the Refal functions corresponding to the states,
each sentence of which corresponds to a transition over a certain symbol.
Probably, the code would be more compact if instead of comparing the next
character with an explicitly specified literal, a search would be performed on
an open e-variable inside a separate set. When it comes to speedwork, it is
difficult to say anything specific about this or that option. As a rule, a
priori (without profiling) it is difficult to determine the optimality of the
solution, especially for such language as a Refal.

* srefc – is, actually, the language compiler itself. It will be further
  discussed precisely in detail.

## \[2] Several methods of lexical analysis have been tested.

### (2.1) Lexical analysis in the preprocessor.
You can notice that most tokens can be represented either as starting with a
character from some set (head) and consisting of some (perhaps, zero) number of
characters from another set (tail), or as starting with a character from a set
for which special tail processing is required. For example, the first type
includes names (begin with the capital Latin and consist of letters, numbers,
hyphens and underscores), numbers (the head and tail sets are the same and
consist of numbers), directives (start with $ and consist of Latin letters),
free space (start and consist of spaces, tabs, and new lines). The second type
includes all the others: variables begin with the letters 's', 'e', 't',
punctuation marks do not have a tail, both types of comments start with '/'
(Simple Refal does not support old Refal comments).

Lexemes of the first type may require some processing procedure (for example,
the directive can be checked for correctness), the second type lexemes simply
require certain processing. Therefore, in principle, you can build a table of
tokens, which will then be interpreted. The table will in principle contain
various processing procedures. I will show you this table.

    BaseTokens {
      = (EOF);

      e.Text =
        <SwBaseTokens
          ( <Default Number> ( <Types::Digits> ) <Types::Digits> )
          ( <Default Space> (' \n\t') ' \n\t' )
          ( <Default Punctuation> ('();{},=<>') )
          ( & FinishName
            (<Types::UpAlph>) <Types::UpAlph> <Types::LoAlph> <Types::Digits> '_-'
          )
          ( & FinishComment ('/') )
          ( & FinishLiteral ('\'') )
          ( & FinishDirective ('$') <Types::UpAlph> )
          ( & FinishVariable ('set') )
          ( e.Text )
        >;
    }

Handlers are not just pointers to a function, but closures from `MLambda`. The
`Default` constructor creates a token handler with the specified name, if
additional actions for processing a line consisting of the head and tail are
not required (in the preprocessor, even the strings of digits are not converted
to numbers, since nothing except output to the output files with lexemes
happens). Strangely enough, the handler of the function name differs from the
default handler, because you need to replace the hyphens with underscores.

### (2.2) Lexical analysis in the generator of the automaton.

The lexical structure of the machine was made simple enough: from complex
constructions there were only state names, set names, dumps and literals of
sets with escape sequences. The names of the states corresponded to the rules
of the Refal language for identifiers, set names – a set of letters and numbers
surrounded on both sides by colons, the names of the flushes began with an
exclamation point followed by the Refal ID for the named reset, the text in the
double quotes for an error message or a hyphen for an unnamed reset. The
lexical analyzer as a whole was built according to the traditional scheme with
little innovation. The matter is that for some alternatives after the part of
the chain that has been read, the choice depends on whether the next symbol
belongs to a certain set. This idea has evolved into the use of the NextInSet
function, which, if the next character belongs to the set, calls one handler,
otherwise the other.

    NextInSet {
      s.SuccessHandler s.FailHandler
      (e.Set-B s.Next e.Set-E) (e.Scanned) s.Next e.Text =
        <s.SuccessHandler (e.Scanned s.Next) e.Text>;

      s.SuccessHandler s.FailHandler (e.Set) (e.Scanned) e.Text =
        <s.FailHandler (e.Scanned) e.Text>;
    }

`s.SuccessHandler` and `s.FailHandler` handlers can also call a function
`NextInSet` recursive. For example.

    LoTokens {
      // ...
      '!' e.Text = <FlushName e.Text>;
      // ...
    }

    FlushName {
      '-' e.Text = (TFlush) <LoTokens e.Text>;

      '"' e.Text =
        <NextInSet
          ErrorFlush ErrorFlushFail
          (<NameTailSet> ' \t,.:;(){}[]*&')
          () e.Text
        >;

      e.Text =
        <NextInSet
          FlushNameTail FlushNameFail
          (<HiLetters>) () e.Text
        >;
    }

    ErrorFlush {
      (e.Scanned) e.Tail =
        <NextInSet
          ErrorFlush ErrorFlushFail
          (<NameTailSet> ' \t,.:;(){}[]*&')
          (e.Scanned) e.Tail
        >;
    }

    ErrorFlushFail {
      (e.Scanned) '"' e.Text =
        (TErrorFlush e.Scanned) <LoTokens e.Text>;

      (e.Scanned) s.Other e.Text =
        (TError 'Expected error text or "') <LoTokens e.Text>;
    }

    FlushNameFail {
      () e.Text = (TError 'expected flush name');
    }

    EndFlushName {
      (e.Scanned) e.Text = (TNamedFlush e.Scanned) <LoTokens e.Text>;
    }

    FlushNameTail {
      (e.Scanned) e.Tail =
        <NextInSet
          FlushNameTail EndFlushName
          ( <NameTailSet> ) (e.Scanned) e.Tail
        >;
    }

When processing a reset-error in the case of `s.SuccessHandler` continues typing
characters, `s.FailHandler` checks whether the next character is a quotation
mark. Similarly, the function FlushNameTail recursively calls itself in case of
success and ends the input of the name if it fails.

### (2.3) Using the lexical analyzer generator.

The third method is used in the Simple Refal's compiler itself. I wrote about
the device of the machine generator in detail above. The main difference
between this way from the other two is that the user does not write the code
itself, but the machine table. The third method is close to the first, because
vocabulary is set by means of a higher level than the code.

Despite the fact that default handlers in the machine are provided (in the case
where no alternative is feasible), I tried to program the machine so these
handlers did not reach the point.

However, after recognizing the lexical structure with the machine, additional
secondary processing of lexemes took place. In particular, the automaton in the
token stream added line feeds – they were only needed to add line numbers to
the tokens, they did not get to the output, all the punctuation marks were
thrown out `(TkPunctuation s.Sign)`, later they turned into various lexemes.

> *Translation to English of this hunk of historical paper is prepared by*
> **Klepikova Valentina <na.etu.pochtu@yandex.ru>** _at 2018-01-17_

### (2.4) The lexical analysis of Modular Refal (v. 0.1.953).

Now I bring the lexical analysis method for comparison into Modular Refal
itself. In Modular Refal the lexical analysis executes the classical scheme
without using functions of the highest order. The module of lexical analysis
was written for a long time ago, at these times I had have the programming
skills only in procedural and object-oriented languages (generally with C++).
That’s why I was implementing lexical analyzer with the following scheme with
using of ADT: creates the abstract scanner type, which after receiving the
NextToken message returns the next lexeme. The scanner itself, in its turn, for
receiving the source text, creates and use another ADT – a symbol stream
(SymStream), encapsulating within the way of reading the file data and
calculating the numbers of strings.

In simplifying the working with defined sets of symbols purposes (for example,
the tail of name symbols – letters, figures, `-`, `_`, `?`, `!`), the symbols
stream class have methods for extracting the sequence of symbols from some set.
The symbol stream class itself includes the file descriptor and read file
string-by-string, as if it needed.

Such architecture decision (scanner class on demand reads the lexemes, symbol
stream class on demand reads the following file strings), what differs from the
first three methods, never fully occupy memory with the file and lexical parse.
That’s why Modular Refal could quite easy (without calculation of lost time)
process the module, which includes 100 Mb of empty strings or 100 Mb of empty
strings with semicolon (what is acceptable to declare within the module) –
extra memory doesn’t required. However, this architecture is more complicated,
as if the whole source text and/or the whole lexical convolution will be
uploaded to memory.

### (2.5) Comparison with different ways.

The most convenient for me is first and third ways: first way mixing up the
sufficient presentation with the code laconicism, third way is more flexible
and high-leveled. To the lacks of first way we could carry, that it in some
grade oriented on table interpreting (for symbol search is needed
double-leveled iteration by open e-variables), what means that, maybe, less
sufficient. The lack of third way is, that code, created as a result, became
too long. The amount of sentences in each function-condition is linear related
from unity of sets power in each of alternatives. Generated code doesn’t
consist of open e-variables, that’s why supposingly it could be sufficient
enough. It could be. But because of code generator in compiler all the function
sentences processing independently, every time it needs a recognizing the same
type format `(e.Accum) '3' e.Text`. As if compiler was able to deduce general
format for several neighbor sentences and in case of samples with similar
structure the general code could be “putted out of commas”, the state machine
will be sufficient enough.

The second variant too well enough, because it’s let us cut the source code
short, putting out the general search code within set to the separate function.
But method isn’t sufficient: as we can recognize by a row of functions, sets
every time require reload, because the sets to handler functions are not
transmitted.

A method with lexical analyzer generator could be mixed up with method of
lexical analysis within Modular Refal – with using the same format of
transfer table describing, it’s possible to create code, which reads lexemes
from file sequentially. That’s why there possible to unite advantages of both
methods: the high-leveled lexeme description and memory economy.

## \[3] Compiler features.
In this laboratory work while I was constructing compiler I check out for
myself some new tricks. In particular, had been used triple-leveled simplified
parsing algorithm and independent generation of different code elements. For
generation of separate sentences under construction was abstract algorithm
(for each sentence) like an imperative command sequence. Later this algorithm
transformed yet to code, and the separate commands were generating almost
independent one of other.

The compiler itself left unfinished for the ready product, but as for the
prototype it’s good enough. From several possibilities, which were ought to be
realized or remake: is standard library catalogs support – into the present
version all the file paths is setting or as absolute, or as relative from the
present folder; the choosing possibility of C like compiler – within present
version program calls program `call_cpp_compiler`, which appears to be the
.bat-like into the present folder; the support of arithmetics at least at the
intermediate level; the support of escape-sequences within the generated code
(with hex-symbols like `\xNN`); overfill checks within integer operations and
a lot of other things.

## (3.1) A brief language overview.

That dialect is practically basis REFAL subset: all the functions presenting
the pairs of sentences `Sample=Result`, terms are only atoms and regular
parenthesis structures (means, that abstract data type, which are nominated
brackets itself, there are absent). The atoms set includes the characters
within, unsigned integers from 0 to 2\*\*32−1 (this aren’t macrofigures),
functions and file descriptors, which could be created only with FOpen function.

Shortly about numbers. The integers doesn’t interpretates as macrodigits of
some limitless by length number, because of library functions Add and Sub have
format `<s.Func s.Num1 s.Num2> == s.Res`, means that it supports only twin atomic
argument (which must be the numbers) and return integer result as single
symbol. If only numbers could be interpreted as macrodigits, the library
functions will be working with rows of numbers. By the way, all the arithmetic
within this dialect is limited only with two functions Add and Sub. For
transforming the number <---> string separate primitive functions used. The
limitation is connected with the fact, that Modular Refal is too limited with
that pair of functions and while I was working on that laboratory work I didn’t
wanted to enlarge it.

The ordering function of twin symbols too haven’t presented, that’s why the
realizing of sufficient escape-consequences support is failed. However for
prototype it isn’t so important.

The representation of functional atom within the program is name of function
itself, and this name ought to be defined. The local and lambda functions
doesn’t exist within language, all the functions should be described
statically into the global scope. All the functions, which present in
translation unit (The modules in meaning of Pascal or Modula in language
doesn’t exist. The program dividing on some separate files of source texts is
corresponds with the meaning of translation unit – the relic, which at first
appeared on Fortran (language was able to support the separate translation, at
expense of what on Fortran was written a lot of libraries, some of which in
still in use nowadays) and after saved within C and C++.) divide on two
classes: local and entry-functions. Those classes correspond functions with
static and external configuration of C/C++ languages.

> _Mazdaywik, 2018-01-18:_ yes, previous paragraph has very ugly language,
> because ugly language has original paragraph in Russian source. Thanks
> to translator (Dmitry Starchenko) for save original style of text.

For the access to functions from the different translation unit using `$EXTERN`
directive, which corresponds with same memory class with the same name in
C/C++. In REFAL programming often in use the symbol names, which have sense of
flags and tags, instead of callable functions. Many (but not REFAL-2 and Simple
Refal) have built-in support the symbol names – atoms of corresponding type is
nominated as identifiers or tags (in literature I met the different
nominations). Simple Refal doesn’t consist of such atoms type. For imitation
of them are used functions. It is obvious, that if function created for using
only as name, the function body never will be (at least mustn’t) called. Such
function could define as empty function, because an empty function with any
argument calls dump (what guarantee program stop in case of (unintentional)
function launch). For cursive writing such functions in language there is
directives `$ENUM` and `$EENUM`. Both functions receive the name list, divided with
commas. The setting up function name in list `$ENUM` is equivalent to declaration
it as empty local, the setting up in list `$EENUM` – as empty entry-function
(eenum – entry enum). For declaration such functions I didn’t use the `$EMPTY`
directive (as in REFAL-2), because _enum_ word more corresponds with their
purpose – the symbol names introduction – as transfer (enum) in such languages
as C++, C#, Visual Basic.

Functions comparing (by the repeated variables way, not in meaning of ordering
function, which is absent) by functions addresses in meaning C language, not by
names (in contrast to REFAL-2 and Refal-7, within which the names is
representate functions and these functions comparing by string representation
of their name). That’s why symbolic names, created in some translation unit
with `$EENUM` assistance, we have to import with the directive `$EXTERN` help.
However, atoms-functions, nevertheless including string name representation –
it is done in debugging purposes, because of sorting out the hexadecimal
address point of view dump extreme difficultness.

The syntax is similar with REFAL 5 syntax with the difference, that the empty
functions (such as `F {}`) is acceptable and exists `$ENUM` and `$EENUM`
directives. There is syntax in EBNF:

    TranslationUnit = Element* .

    Element =
      '$ENUM' NameList |
      '$EENUM' NameList |
      '$EXTERN' NameList |
      '$ENTRY' Function |
      Function .

    NameList = Name ',' NameList | Name ';' .

    Function =
      Name '{' Sentence* '}'

    Sentence = Pattern '=' Result ';' .

    Pattern = PatternTerm* .

    PatternTerm = Char | '(' Pattern ')' | Number | Name | Variable .

    Result = ResultTerm* .

    ResultTerm = PatternTerm | '<' Result '>' .

    Pattern = PatternTerm* .

I won’t be bringing the lexis, because of it’s easy to understand from the
state machine table, with which had been constructing lexical analyzer.

Some features of lexical analysis. The functions names and variables indexes
could include hyphen, which fully equivalent with underlining. The need of
replacing `-` to `_` linked with the fact, that C/C++ language didn’t support
names with hyphen, but the source texts based on REFAL looks pretty good with
usage of hyphen in names. When reading integer variable overfilling check
hadn’t started – at excess of size 2\*\*32−1 the result appears to be
undefined.

Within language strangely enough, was no place for the global variables: I’ve
got used to program on REFAL without them, that’s why compiler complication
with useless remedy I didn’t do. In case of sudden need, I able to write on C
stack support.

## (3.2) Several words about regular functions.

It is obvious, that with REFAL remedies (identification with the sample and
result constructing) a number of operations execution is impossible: it is I/O
operations and atom operations. Of course, the arithmetic functions could be,
theoretically, described in that way:

    Add {
      0 0 = 0;
      0 1 = 1;
      1 0 = 1;
      ...
      234 456 = 690;
      ...
    }

But even if functions of such kind will be written (with auto-generation, to
example with program based on C), then practical using of them will be
complicated – they extremely insufficient. That’s why language need primitive
functions, executing primitive operations.

Within actual compiler accepted the following model. Inputing the built-in
functions to language is ideologically disgusting for me, because the functions
is related to user-level definitions, but execute language-level actions.
Besides for the built-in functions executes special rules, for instance, they
don’t need to introduce. That’s why I made the primitive operations as external
– as in C like language – there is too absent built-in functions. Because of
being the primitive operations (those, that impossible create with REFAL
sources) is external, than exactly user too able to write a him/herself
primitive functions based on C++ language and it too will be external.

From the standard external functions to us given the following:

* Standard tag-names: `Success`, `Fails`, `True`, `False`.

* Arithmetic:
  * `Add`, `Sub` – arithmetical operations.

* Input/output:
  * `<ReadLine> == e.Line` – reads the input from console,
  * `<WriteLine e.Line> == empty` – prints to console,
  * `<FOpen s.Mode e.Name> == s.FileHandle` – opens file with preset name
    within preset configuration: `'r'` – for reading, `'w'` – for writing.
  * `<FReadLine s.FileHandle> == s.FileHandle e.Line`,
  * `<FWriteLine s.FileHandle e.Line> == s.FileHandle`,
  * `<FClose s.FileHandle>` – close the file.

The input/output file functions made on an image of corresponding with module
FileIO functions in Modular Refal, because for developing preprocessor as for
the primitive operations I used library functions of Modular Refal.

* Types transforming:
  * `<StrFromInt s.Int> == e.Digits` – transforming number to string,
  * `IntFromStr`:

    ```
    <IntFromStr e.Text>
    == Success s.Number e.Rest
    == Fails e.Text
    ```
    If only `e.Text` begins from the numbers chain, that chain transforms
    to number `s.Number` and returns remains as `e.Rest`. If transforming
    is impossible, function returns `Fails` and own argument.
  * `<Chr s.Num> == s.Char`, `<Ord s.Char> == s.Num` – obvious.

* OS capabilities.
  * `<System e.Command> == empty` – call this command line,
  * `<Exit s.ReturnCode>` – ends program with distribution return code,
  * `<Arg s.Number> == e.Argument` – return argument of command line with
    preset number,
  * `<ExistFile e.FileName> == True | False` – file presence check.

Also to compiler attached regular library extension LibraryEx, which fully
written on REFAL and includes such useful functions, as `Map`, `Reduce`,
`MapReduce`, `LoadFile`, `SaveFile`, and many others. I won’t bring their
description, because semantic is easy to understand from the source code.

## (3.3) Syntax analysis features.

We can recognize, that if nonterminals `PatternTerm` and `ResultTerm` will be
described in following way (with loss of balanced brackets), the grammar will
become practically regular (not by appearance, but easy to transform to
regular).

    PatternTerm = Char | '(' | ')' | Number | Name | Variable .

    ResultTerm = PatternTerm | '<' | '>' .

And describe such state machine will be much easyer (in REFAL it is surprisingly
easy to write state machine programs). After finishing the pattern and result
analysis it is possible to separately check the brackets balance in them. In
program I went much further: in separate procedure I took out the variables
check too (within pattern inacceptable the meeting of two variables with
different types (s,t or e) with the same index, in result expression can’t
exist unrepresented into the pattern variables).

This state machine worked as transducer – by the reading of separate
elements (directives `$ENUM`, `$EENUM`, `$EXTERN`, the functions beginning with
curly bracket, the separate sentence, enclosing curly bracket) momentarily
generated code (in comparison with extern – C like function representing, empty
functions generation, which ends with failure, with the different memory
classes, the non-empty function beginning, sentence processing code, function
finishing). The separate element generation was fully independent one of other,
which I got to consider while I was creating the output files. The exception
was only symbol table, which consist of functions names – for the fact check,
that all names are ought to be represented before the first executing. About
generation I will tell later.

As it has appeared, this syntax analysis dividing to the three ways (which were
carried out only by separate sentences, not by the whole transmition length)
drastically simplified the compiler structure. Such driving to the state machine
became possible in order with the fact, that terms, from which consists left
and right sentences parts, can’t include dividers (corresponding, ‘=’ and ‘;’).
If only that dialect could support such nameless or local functions as terms
( which includes within sentences, dividing with assistance of ’=’ and ’;’),
then such driving of context-free grammar to regular would be imposible.

To sum it up, practice show, that dividing on two ways the free context syntax
and context relations is very useful – the compiler structure became easyer.
At paired brackets relations check we could don’t care about unexpected lexemes,
at variables presence check – about brackets paired relations.

 In that case independent-context syntax check, which easy to formalize, could
be executed automatically – it is possible to write syntax BNF analyzer
generator, fabricating the syntax tree. Further that tree could be bypassed by
context relations check and construct abstract syntax representation.

## (3.4) Virtual REFAL-machine implementation.

The virtual REFAL-machine was created at classical scheme with point of view
usage. Point of view is the double-linked node list, each of which represents
the atom, or this or that bracket (one from `(`, `)`, `<`, `>`). Nodes includes
links to the neighbor-nodes, type field and info field, which is union of
several fields with different types. Numbers-nodes in info-field include
`unsigned long`, symbol-nodes – `char`, function-nodes includes function
pointer and `const char` pointer, including function name. Nodes, which
corresponds to structural brackets, includes the linked brackets relations.
Call brackets pointers situated in calls stack in that order, in which they got
to execute. If function call is replacing with active expression with the
different evaluation brackets, then evaluation brackets will be putted on top
of stack in proper order – so realizing saving stack invariant. For
implementation of the stack is used info field in brackets nodes (as in REFAL
2).

C++ function, corresponding to REFAL function, when performing it could or
replace the own call within the point of view, having returned refalrts::
cSuccess, or end with failure because of recognition impossibility or lack of
memory, having returned refalrts::cRecognitionImpossible; in the end of
function.

Within function is composed of separate sentences handlers, each of which
could or successfully complete the function (with performing `return
refalrts::cSuccess`) or message about lack of memory (with performing `return
refalrts::cNoMemory`). If handler doesn't ends the function with one of those
messages, the control takes next handler, in case of handlers depleeting –
to the statement performs `return refalrts::cRecognitionImpossible`; in the
function end.

The executing of sentence processor passes through three phases:

1. Argument recognition. At the argument recognition process argument remains
   without change. If recognition is impossible, the exit procedure executes.
2. Variables replicating and allocating the result elements, which preset
   by literals, including structural and functional brackets (the ready brackets
   and atoms from the pattern are not in use – it’s done in compiler simplifying
   purpose). At this stage function could produce message `refalrts::NoMemory`.
   New elements allocating within the free blocks list (see below). The function
   argument at abnormal termination doesn’t changes.
3. Building of result from the pattern variables and elements, result
   constructing goes with the assistance of operations on double-linked lists,
   which doesn’t violate invariant – the part of a row transmitting with
   exception from the source splicing. After result constructing all what left
   within the pattern is moving to the free blocks list. All the operations in
   that phase can’t fail – that phase ends with the `refalrts::cSuccess`
   returning.

   For the memory optimization there in use the double-linked list of free
   nodes. All the operations, giving memory (copying the variables or
   constructing new nodes), creates own argument within that list. Later the
   list parts with rebuilded elements transmits into the field of view. Such
   strategy provides consistency of both lists at raising a message
   `refalrts::cNoMemory` – created elements remain in free blocks list,
   argument doesn’t change.

## (3.5) Sentences generation.

Sentences generation proceed in two stages: from the very beginning by
intermediate representation creates processor based on abstract imperative
language (means, that sentence transforms to the sequence of procession
commands), and then processor rebuilds from abstract imperative representation
to C++, and the separate commands trasmiting practically independently one of
other. The last two phases operations: giving new memory and result fabrication
realizing not so difficult, but the pattern recognition is very interesting.

I bring rules of pattern matching so, as it shown in REFAL-5 guide, and then
comment, how they realized in real compiler.

### General requirements to mapping P on E (matching E : P)

1. If a node N2 is positioned in P to the right of a node N1, then the
   projection of N2 in E can either coincide with, or be positioned to the
   right of, the projection of N1 (projection lines cannot cross).
2. Projections of brackets and symbols must be identical to themselves.
3. Projections of variables must meet the syntax requirements of their values;
   i.e., to be symbols, terms, or arbitrary expressions for s-, t-, and
   e-variables, respectively. Different entries of the same variable must have
   equal projections.

> Dmitry Starchenko’s independed translation:
> ### The general requirements for P at E representing (comparing E : P)
>
> 1. If node N2 situated in P at the right side of node N1, the projection N2
>    in E could or compare with N1 projection, or be situated from the right
>    side of it (projection lines can’t be crossed).
> 2. Brackets and symbols should be the same with their projections.
> 3. Variables projections ought to compare with syntax requirements; means,
>    be the symbols, terms or free expressions for s-,t- and e- variables. The
>    different inputs of the same variable should have same projections.

> *Translation to English of this hunk of historical paper is prepared by*
> **Starchenko Dmitry <starchenko_dmitry@mail.ru>** _at 2018-01-07_

### Rules of mapping

1. After a bracket is mapped, its pairing bracket is mapped next.
2. If, as a result of previous steps, both ends of an entry of an e-variable
   are already mapped, but this variable has no value yet (no other entry of it
   has been mapped), then this variable is mapped next. Such entries are called
   closed e-variables. Two closed e-variables can appear at the same time; in
   this case, the one on the left is mapped first.
3. An entry of a variable which already has a value is a repeated entry.
   Brackets, symbols, s-variables, t-variables, and repeated entries of
   e-variables in P are _rigid_ elements. If one of the ends of a rigid element is
   mapped, the projection of the other end is unique. If Rules 1 and 2 are not
   applicable, and there are some rigid elements with one end projected, the
   leftmost of these is chosen. If it is possible to map this element without
   contradicting the general requirements 1-3 above, then it is mapped, and the
   process goes on. Otherwise, a _deadlock situation_ is stated.
4. If Rules 1-3 are not applicable, and there are some e-variables with the
   left end mapped, then the leftmost one is chosen. It is called an _open_
   e-variable. Initially it gets an empty value, i.e., its right end is
   projected on the same node as the left one. Other values may be assigned to
   open variables through lengthening (see Rule 6).
5. If all elements of P are mapped, the matching process is successfully
   completed.
6. In a deadlock situation, the process comes back to the last open e-variable
   (i.e., the one with the maximal projection number) and its value is
   lengthened; i.e., the projection of its right end in E is moved one term to
   the right. Thereafter the process is resumed. If the variable cannot be
   lengthened (because of the General requirements 1-3), the preceding open
   variable is lengthened, etc. If there is no open variable to be lengthened,
   the matching process fails.

> Anastasia Solomatina’s independed translation:
>
> ### The display rules
>
> 1. After that the bracket is displayed, the matching parenthesis will be
>    displayed the next to it.
> 2. If, as a result of the previous steps, both ends of the occurrence of some
>    e-variable are already displayed, but this variable hasn’t value (no other
>    of its occurrences was displayed), then this variable is displayed next.
>    Such occurrences are called closed e-variables. Two closed e-variables can
>    appear at the same time; in this case, the one on the left is displayed
>    first.
> 3. The occurrence of a variable that already has a value is a repeat.
>    Brackets, symbols, s-variables, t-variables and repeated occurrences of
>    e-variables in Refal are rigid elements. If one of the ends of a rigid
>    element is displayed, the projection of the second end is uniquely
>    determined. If Rules 1 and 2 are not applicable, and there are several
>    rigid elements with one
>    projected end, then the left one is selected from them. If it is possible
>    to display this element without colliding with the general requirements
>    1-3 given above, then it is displayed and the process continues on.
>    Otherwise, a deadlock is declared.
> 4. If Rules 1-3 are not applicable and there are several e-variables with the
>    left end displayed, then the leftmost one is selected. It is called an
>    open e-variable. Initially, it gets an empty value, i.e., its right end is
>    projected to the same node as the left one. Other values may be assigned
>    to the open variables via elongation (see Rule 6).
> 5. If all the elements of P are displayed, it means that the matching process
>    has been successfully completed.
> 6. In a deadlock situation, the process returns back to the last opened
>    e-variable (i.e., to the one that has the maximum projection number), and
>    its value is extended; i.e., the projection of its right end in E moves
>    one term to the right. After that, the process restarts. If the variable
>    cannot be extended (due to General Requirements 1-3), the previous open
>    variable is
>    extended, etc.  If there are no open variables to be extended, the
>    displaying process failed.

However, it is obvious that the order of displaying rigid elements and closed
e-variables is insignificant from the point of view of the final result. Only
the order of displaying of open e-variables is important. Therefore, in my
case, when creating an algorithm for pattern recognition (an intermediate
abstract imperative representation in the code is called an algorithm), the
order of displaying rigid elements is slightly different than in the Rules
described above.

The algorithm for pattern recognition is modeled this way. Consider the method
for the case of a “flat” expression – an expression without the parentheses,
and then generalize to a hierarchical case. Two pointers are entered in the
pattern – left and right (we denote them as `[` and `]`). These pointers can be
found between the individual terms of the pattern or directly to the right or
left outside the pattern (this is the original position of the right and left
pointers respectively).

Next, the pattern is viewed interactively and the pointer is moved according to
the following rules:

1. If to the right of `[` there is a rigid element, then a command is created
   to recognize this hard element from the left end of the expression and the
   pointer is shifted one element to the right.
2. Similarly, if there is a rigid element to the left of `]`, then the command
   is created to recognize this hard element from the right end and the pointer
   is shifted one element to the right.

   In this case, if a variable is recognized by following rules 1) or 2), then
   it is remembered as recognized and subsequently identified as a repeated
   one.
3. If there is an unrecognized e-variable between the pointers: [e.Index], then
   a command is created to identify the remainder of the expression with a
   given e-variable. Thereby, the process of recognizing the expression is
   completed.
4. If the `[` and `]` pointers are encountered, a command is created to check
   the remaining expression for emptiness.
5. If the unrecognized e-variables are to the right of the pointer `[` and to
   the left of the pointer `]`, then the command-marker of the open e-variable
   is created and the pointer `[` is shifted to the right by one element. In
   addition, all the commands between this marker and to the end are put in the
   nested loop, in which the open variable is incremented by the term for each
   iteration and the remaining commands are tried an execution.

For example,

    s.X 'a' 2 e.Y s.Z
    [ s.X 'a' 2 e.Y s.Z ] => svar_left(s.X), s.X [ 'a' 2 e.Y s.Z ]
    s.X [ 'a' 2 e.Y s.Z ] => char_left('a'), s.X 'a' [ 2 e.Y s.Z ]
    s.X 'a' [ 2 e.Y s.Z ] => numb_left( 2 ), s.X 'a' 2 [ e.Y s.Z ]
    s.X 'a' 2 [ e.Y s.Z ] => svar_right(s.Z), s.X 'a' 2 [ e.Y ] s.Z
    s.Z 'a' 2 [ e.Y ] s.Z => rest_evar(e.Y), end of recognition
    s.X 5 t.Z
    [ s.X 5 t.Z ] => svar_left(s.X), s.X [ 5 t.Z ]
    s.X [ 5 t.Z ] => numb_left( 5 ), s.X 5 [ t.Z ]
    s.X 5 [ t.Z ] => tvar_left(t.Z), s.X 5 t.Z [ ]
    s.X 5 t.Z [ ] => rest_empty, end of recognition
    1 e.X 2 e.Y 3
    [ 1 e.X 2 e.Y 3 ] => numb_left( 1 ), 1 [ e.X 2 e.Y 3 ]
    1 [ e.X 2 e.Y 3 ] => numb_right( 3 ), 1 [ e.X 2 e.Y ] 3
    1 [ e.X 2 e.Y ] 3 => E_CYCLE( e.X ), 1 e.X [ 2 e.Y ] 3
    1 e.X [ 2 e.Y ] 3 => numb_left( 2 ), 1 e.X 2 [ e.Y ] 3
    1 e.X 2 [ e.Y ] 3 => rest_evar(e.Y), end of recognition

This method is easily extended to the case of a parenthesis structure: pointers
are entered not only for the entire pattern, but for each separate pair of
parentheses.

To distinguish expressions in different parentheses, each individual pair of
brackets can be assigned an individual integer index starting at 1. Similarly,
the pointers receive their own individual index, while the full pattern receives
pointers with index 0.

The recognition mechanism for such a case is described by a pseudo-code on
Refal:

    PatternMatching {
      // Recognition on the left

      //1
      e.Left [_N t.Атом e.Right =
        atom_left( t.Atom, expression N )
        <PatternMatchign e.Left t.Атом [_N e.Right>;

      //2
      e.Left [_N t. repeated_variable e.Right =
        repeatedvar_left( t.repeated_variable, expression N )
        <PatternMatching e.Left t. repeated_variable [_N e.Right>;

      //3
      e.Left [_N t.st-переменная e.Right =
        stvar_left( t.st- variable, expression N )
        Remember t.st-variable as recognized
        <PatternMathcing e.Left t.st- variable [_N e.Right>;

      //4
      e.Left [_N (_M e.Inner )_M e.Right =
        initialize expression M
        bracket_left( expression M, expression N )
        <PatternMatching
          e.Left (_M [_M e.Inner ]_M )_M [_N e.Right
        >;

      // Recognition on the right

      //5
      e.Left t.Atom ]_N e.Right =
        atom_right( t.Atom, expression N )
        <PatternMatching e.Left ]_N t.Atom e.Right>;

      //6
      e.Left t. repeated_variable ]_N e.Right =
        repeated_right( t.repeated_variable, expression N )
        <PatternMatching e.Left ]_N t. repeated_variable e.Right>;

      //7
      e.Left t.st- variable ]_N e.Right =
        stvar_right( t.st- variable, expression N )
        <PatternMatching e.Left ]_N t.st- variable e.Right>;

      //8
      e.Left (_M e.Inner )_M ]_N e.Right =
        initialize expression M
        brackets_right( expression M, expression N )
        <PatternMatching
          e.Left ]_N (_M [_M e.Inner ]_M )_M e.Right
        >;

      // Pointer annihilation and open e-variables

      //9
      e.Left [_N t.closed-e-variable]_N e.Right =
        closed_e( t.closed-e-variable, expression N )
        Remember  t.closed-e-variable as recognized
        <PatternMatching e.Left t.closed-e-variable e.Right>;

      //10
      e.Left [_N ]_N e.Right =
        empty_expression( expression N )
        <PatternMatching e.Left e.Right>;

      //11
      e.Left [_N t. unrecognized-e-variable-1 e.Inner
      t.unrecognized-e-variable-2 ]_N e.Right =
        E-CYCLE( t.unrecognized-e-variable-1, expression N )
        Remember  t.unrecognized-e-variable-1 as recognized
        <PatternMatching
          e.Left t.unrecognized-e-variable-1
          [_N e.Inner t.unrecognized-e-variable-2 ]_N e.Right
        >;

      // the loop is executed-- there are no pointers

      //12
      e.Pattern = ;
    }

### Initialization

Initialize expression 0 with an argument

    <PatternMatching [_0 e.Pattern ]_0>;

In the pseudo code, the notation `[_N`, `]_N`, `(_N` , `)_N` denoted the
numbered pointers and the parentheses, respectively. Up to the order of the
displaying of rigid elements, this algorithm generates the order of the
displaying generated by Rules 1-6. Let us define it.

Rule 1 is automatically executed if the primitive recognition commands, from
which the algorithm is created, are recognized by two parentheses at the same
time. The way it is in the real compiler.
Sentences 4 and 8 just generate abstract commands that perform this function.

For generality of reasoning, let us assume that the argument to be recognized
is immersed in a pair of parentheses with the number 0: `(_0 e.Pattern)_0`.
This will simplify the description, because it will not be necessary to
clearly distinguish the cases of the location of the elements directly next to
the edge. Thereby, at the beginning of processing the pattern has the form

    (_0, [_ 0 e.Pattern] _0) _0

It can be convinced that the pointers break each sub expression into three
parts: the right and left are already designed elements, immediately next to
the pointers are the elements with one projected end, and between them the
elements with free ends.

If there are rigid elements with a projected left end, then sentences 1-4 will
create commands to recognize them. Similarly, if there are rigid elements with
a projected right end, then they are handled by sentences 5-8. Thus, up to the
order of recognition of rigid elements, propositions 1-8 implement rule 3.

If there is a closed e-variable, then it will be processed by sentence 9 -
implementation of rule 2.

Rules 4 and 6 are implemented already in the most generated code – all commands
starting with the marker command `E-CYCLE` and up to the end of the algorithm
for constructing the expression are placed in the nested loop (if there is
another `E-CYCLE` marker after the `E-CYCLE` marker, then the code between them
and the end is placed in another nested loop, etc.). Before the loop is
executed, all the pointers that can change in the cycle are stored, at the
beginning of each iteration the pointers are restored, at the end of each
iteration the elongation of the open e-variable is extended by one term. If
pattern recognition is possible, then phases 2 and 3 are executed, which can be
completed only by returning `refalrts::cSuccess` and `refalrts::cNoMemory`.

If recognition by iteration is not possible with the e-variable, the next
iteration by the continue statement of the C++ language is started. By
unsuccessful recognition outside the iteration, it is necessary exit the
handler of the sentence and go to the next clause or command return
`refalrts::cRecognitionImpossible`.

Each sentence handler is inside the `do {...} while (0);` loop, therefore, you
can exit the handler by executing the break statement outside the loops by
e-variables. So the handler looks like this:

    do {
      // ... Recognition outside cycles by e-variables
      if( at some step the recognition is not possible )
        break;

      // iteration over an open e-variable
      // ... Saving the required pointers
      for(
        cycle initialization;
        while elongation is possible;
        elongation by term
      ) {
        // ... Restore the required pointers

        // ... Recognition within a cycle
        if( at some step the recognition is not possible )
          continue;

        // The pattern is disassembled – phase 2 – allocation of memory

        if( at some step the memory was not enough )
          return refalrts::cNoMemory

        // ...The results – phase3
        return refalrts::cSuccess;
      }
    } while(0);

Rule 5 is done through the implementation of sentence 12 – if not left pointer
that are next to unrecognized elements (the preceding sentences handle all
possible cases of pointer locations), the recognition is completed.

The sub ranges of the argument and result are represented by a pair of
iterators of a doubly connected list (usual pointers to nodes). The first
iterator points to the first node of the sub band, the second iterator to the
last node. An empty sequence is represented by a pair of iterators set to
`NULL`.  In the beginning, I tried to use the STL-style range designation -
pointers to the first element of the element following the last one.

However, for correct handling of such sub bands, it is necessary carefully plan
the command sequences constructing the result; as a result of the transfer
(splicing) of the elements immediately beyond the range in question, the element
pointed to by the end iterator will move – the `[first, last)` pair will no
longer indicate the correct range. When ranges `[first, last]` is used
iterators that point to current range will not be changed when any operations
with adjacent ranges is performed.

The operations for recognizing the rigid elements of the pattern are represented
by elementary functions with the suffixes `_left` and `_right` (with the
exception of `move_left` and `move_right`).

They all have roughly the same format:

    bool ***_left(description the rigid element, Iter& first, Iter& last);
    bool ***_right(description the rigid element, Iter& first, Iter& last);

`***` – type of a rigid element (function name, number, character, structured
parentheses, repeated variable, st-variable).

The description of the rigid element is represented by setting of parameters
characterizing a rigid element (meaning for atoms, links to the right and left
end of the sub expression for the structured parentheses, term for s- and
t-variables, for repeated variables – the description of the pattern (to a term
for st-variables, a pair of iterators for e-variables) and putting the variable
itself (reference to the iterator for st- and the pair for e-variables-)).

The functions return true in the case of successful recognition, moving with
the iterators first and last so that the newly created range `[first,last]`
pointed to the unrecognized part of the expression.

 For example,

      'abcdef' => { char_left } => 'bcdef'
      F G H => { function_right } => F G

If you cannot recognize a rigid element, it returns false, and the associated
list of the field of view does not change, the values of the variables passed
by links are not changed.

The precondition for these functions is the correct specification of the
parameters for the description of the rigid element and the correct indication
of the subband `[first, last]` (indicate the ends, or both are zero).

The postcondition is, in the case of a well-designed algorithm, compliance with
general requirements 1-3.

Iteration uses a simple for loop on an open e-variable. If, at some iteration,
by some length of the e-variable, the recognition the code for allocating
memory and constructing the result is executed, which can be completed only by
issuing `refalrts::cNoMemory` or `refalrts :: cSuccess`.  In case of
unsuccessful recognition of the remaining elements of the pattern, the
iteration is terminated with the continue operator and the e-variable is
extended.

If the variable cannot be lengthened, then the progress to the end of the
sentence handler occurs: if this cycle is not nested in another cycle in the
e-variable, then the next sentence is executed, otherwise – the iteration of
the outer loop over the e-variable is completed (elemental from for the fact
that we have reached the end of the body of the cycle).

Pseudocode:

    do {
      // recognition up to an open e-variable
      ...
      if( somewhere the recognition failed )
        break;
      ...
      for(
        /* Initialization */;
        /* Stretching is possible further */;
        /* Variable extension */;
      ) {
        ...
        if( somewhere the recognition failed )
          continue;
        ...
        for(
          /* Initialization */;
          /* Stretching is possible further */;
          /* Variable extension */;
        ) {
          ...
          // Allocating memory
          ...
          if( no memory )
            return refalrts::cNoMemory;
          ...
          // The results
          ...
          return refalrts::cSuccess;
        }
      }
    } while( 0 );

> _Translation to English of this hunk of historical paper is prepared by_
> **Anastasia Solomatina <solomyash@gmail.com>** _at 2018-01-19_
> _Markdown is prepared by_ **Liudmila Markova <luckymarkin@gmail.com>**
> _at 2018-01-19_

If recognition is impossible, the rollback with the recovery of the previous
state (implementation of 6 Rule) should be implemented; because variables of a
type `bb_N` and `be_N` (the boundaries of subexpressions in brackets) change
during the recognition. For the value’s recovering are used the following
property of the C++ language – the ability to declare variables in nested
blocks with the same name as it is used in the outer block with the concealment
of the latter.  In that way, in the block of for-loop initialization it defines
the variables `bb_N` and `be_N` of the refalrts `type::Iter`, which are
initialized as the variables of the same name in the outer block. Thus, to save
a value, you do not have to start variables with new names or nested variables
to give other names (this would have to be done when generating Pascal code).
But, since the result of the initialization `refalrts::Iter bb_N = bb_N;` is
not defined, it is necessary to establish an intermediate variable:

    refalrts::Iter bb_N_stk = bb_N;
    refalrts::Iter be_N_stk = be_N;
    for(
      refalrts::Iter
        ...
        bb_N = bb_N_stk,
        be_N = be_N_stk,
        ...;
      ...;
      ...;
    ) {
      ...
    }


Unfortunately, there had been made an error. This mechanism works perfect, but
it is saved only the value of the boundaries of such subexpression, which
consists opened e-variable. If the boundaries of other subexpressions are
modified within the iteration, then these changes are retained and further
recognition is incorrect. For example, during the recognition of

    e.Begin (_1 e.Inner)_1 (_2 e.Any 'X' e.Inner)_2

pattern the boundaries of the pair of parentheses number 2 are modified, which
leads to the matching impossibility, for example with the
`('er') ('super') ('any' 'X' 'super')` expression.

Basically, this error can be rectified easily by saving all recognition
parentheses. (even if they are not modified),but I can't be bothered. I'm not
going to follow this compiler (at least in the foreseeable future).

Also, it would make sense to keep `***_stk` variables not only as pointers
(`refalrts::Iter` – a synonym (`typedef`) for pointers), but as references
to them (`refalrts::Iter&`) – according to the Standard reference types are
defined just as another name of the variable – so memory allocation for them is
not necessary to happen. But I believe in the optimization’s power of modern
compilers, which are able to eliminate intermediate assignment in the
`b = a; c = b;` chain and if `b` is not used anywhere else and is a simple
built-in data type like pointer.

However, I am(not) lucky: the compiler successfully compiles itself and it
works properly despite this error. If this error led to incorrect results, then
I would have to fix it and the outcome would have for one mistake less.


### (3.5) Constraints of the research prototype.

The result is only research prototype, it is not intended for using it in real
life. So, I left it unfinished. The language disadvantages (requirement of
forward declaration of functions, the lack of modularity and the C style
compositing model, using only basic subsets support, etc.) I don't count these
as deficiencies, because it is just the language weaknesses, but not the
compiler mistakes. Here is a list of deficiencies.

* (1) All source files must be listed in the command line by their absolute
  pathname or partial pathname. There is not such a great opportunity as
  specifying the directory for libraries searching.
* (2) The absence of configuration tools. The `call_cpp_compiler` program is
  triggered for compilation (by using `System` function), which is implemented
  as the bat file, which actually leads the compiler.
* (3) There are uncorrected errors such as state saving error during the
  rollbacks in open e-variables (see above).
* (4) The Code 	creating is rather straightforward: certain instructions of the
  abstracted imperative language (the algorithm) are generated independently
  from each other. Often, the variables overlap (e.g., in the presence of
  closed e-variables inside the parentheses we could avoid using of `bb_N` and
  `be_N` iterators).
* (5) During further research on algorithm elaboration, it also could be used
  the STL style ranges. In doing so, it would be possible not to use
  `eVarName_e` variables to indicate the end of e-variables – the end in this
  case was limited by the next variable.
* (6) The re-use of the literal elements of the sample (atoms or brackets)
  instead of selecting them from the list of free blocks it also could be used.
  Also, you can reuse the unused literal elements and st-variables by
  re-initializing them.
* (7) As we look at the RTS implementation, we can see that the code
  recognition of individual atoms, pairs of parentheses, repeated variables are
  largely similar with functions `***_left` and `***_right`, which are similar
  as twins-brothers. In principle, the compiler instead of calling functions
  code inserting could insert the operators representing the function body.
  Although this would lead to a sharp increase in the volume of the generated
  code.
* (8) There is no optimization. Due to the fact that certain statements are
  generated independently from each other, the same operation (in the case that
  a function has a specific format of the argument – what happens almost every
  time) are executed repeatedly. The ineffectiveness of this can be seen if you
  look at the automaton code of lexical analyzer in `Lexer.sref` and `Lexer.cpp`
  files. Long shot, that the same calculations will be compiled by a C++
  compiler – during recognition it is called an external function (see
  section 7), which could potentially have a side-effect. Although, if the
  statements were processed collaboratively, linear handler enumeration could
  be replaced with the tree. That means, instead of


      /*
        All three sentences have the common format.
        Also the first two sentences have much in common in the pattern
      */

      do {
        // recognition of the first sentence
        if ( recognition failure )
          break;
        // result building
        return refalrts::cNoMemory;
        return refalrts::cSuccess;
      } while(0);

      do {
        // recognition of the second sentence
        if( recognition failure )
          break;
        // result building
        return refalrts::cNoMemory;
        return refalrts::cSuccess;
      } while(0);

      do {
        // recognition of the third sentence
        if( recognition failure )
          break;
        // result building
        return refalrts::cNoMemory;
        return refalrts::cSuccess;
      } while(0);

      return refalrts::cRecognitionImpossible;

  We might write

      do {
          // recognition of common to the three format proposals
          if( recognition failure )
            break;

          do {
            // recognition of the common elements for the first two sentences
            if( recognition failure )
              break;

            do {
              // recognition of the first sentence
              if( recognition failure )
                break;
              // result building
              return refalrts::cNoMemory;
              return refalrts::cSuccess;
            } while(0)

            do {
              // recognition of the second sentence
              if( recognition failure )
                break;
              // result building
              return refalrts::cNoMemory;
              return refalrts::cSuccess;
            } while(0)

          } while(0);

          do {
            // recognition of the second sentence
            if( recognition failure)
              break;
            // result building
            return refalrts::cNoMemory;
            return refalrts::cSuccess;
          } while(0);

        } while(0);

        return refalrts::cRecognitionImpossible;

  In this case, after the recognition failure of the first sentence in the
  second we will have to complete a minimum of computations for recognition,
  because part of the expression is already recognized.

* (9) Meager standard external (“built-in”) functions library. It is actually
  possible to address the deficiency easily.

# Conclusions.

* \[1] I have written the compiler, compiling Refal into C++.  The results of
  Refal → C++ translation research results can be found in the Results section
  of this file. Those language features that were planned to Goal of the lab
  were implemented. The resulting compiler works, although not without flaws
  (see above).
* \[2] It was written the self-hosted compiler, this means that the language
  can be used to write non-trivial enough applications (such as compiler) and
  not just programming of `Hello, World` and `Fibonacci`.
* \[3] During some errors and defects fixing process, the language can be used
  also as the C++ back end for Modular Refal (of course, by adapting it under
  another level of code modularity). And can be used in two ways: (1) the
  generation of files in a Simple Refal and then running of the Simple Refal
  compiler – in the same way with the back end Refal-5 compilation and
  (2) the code integration of the Simple Refal into the Refal Modular compiler.
  The second option is facilitated by the fact that we have a preprocessor,
  compiling a Simple into Modular Refal.
* \[4] Developing of lexical generators and syntactic analyzers would be
  useful. It is truly  comfortable
* \[5] If (when) front-end for a Modular Refal will be developing, generating
  code for imperative languages, it is advisable to use the STL style borders
  and more careful planning of the generated algorithm.

> *Translation to English of this hunk of historical paper is prepared by*
> **Anna Mazura <annaeav@yandex.ru>** _at 2018-01-18_
