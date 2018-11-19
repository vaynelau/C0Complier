program PASCALS(INPUT,OUTPUT,PRD,PRR);
{*  author:N.Wirth, E.T.H. CH-8092 Zurich,1.3.76 *}
{*  modified by R.E.Berry
    Department of computer studies
    University of Lancaster
    Variants of this program are used on
    Data General Nova,Apple,and
    Western Digital Microengine machines. *}
{*   further modified by M.Z.Jin
    Department of Computer Science&Engineering BUAA,0ct.1989
*}
const
  nkw = 27;    {* 保留字的个数 *}
  alng = 10;   {* 标识符有效字符的最大长度 *}
  llng = 121;  {* input line length *}
  emax = 322;  {* max exponent of real numbers *}
  emin = -292; {* min exponent *}
  kmax = 15;   {* max no. of significant digits *}
  tmax = 100;  {* size of table *}
  bmax = 20;   {* size of block-talbe *}
  amax = 30;   {* size of array-table *}
  c2max = 20;  {* size of real constant table *}
  csmax = 30;  {* max no. of cases *}
  cmax = 800;  {* size of code *}
  lmax = 7;    {* maximum level *}
  smax = 600;  {* size of string-table *}
  ermax = 58;  {* max error no. *}
  omax = 63;   {* highest order code *}
  xmax = 32767;  {* 2**15-1 *}
  nmax = 32767;  {* 2**15-1 *}
  lineleng = 132; {* 输出行长度 *}
  linelimit = 200; {*行数限制*}
  stacksize = 1450; {*数据栈大小*}
type
  symbol = (intcon, realcon, charcon, stringcon,
    notsy, plus, minus, times, idiv, rdiv, imod, andsy, orsy,
    eql, neq, gtr, geq, lss, leq,
    lparent, rparent, lbrack, rbrack, comma, semicolon, period,
    colon, becomes, constsy, typesy, varsy, funcsy,
    procsy, arraysy, recordsy, programsy, ident,
    beginsy, ifsy, casesy, repeatsy, whilesy, forsy,
    endsy, elsesy, untilsy, ofsy, dosy, tosy, downtosy, thensy); {*单词符号的各种类型*}
  index = -xmax.. +xmax; {*index类型的取值*}
  alfa = packed array[1..alng] of char; {*一个常用长度为10的字符串类型*}
  objecttyp = (konstant, vvariable, typel, prozedure, funktion); {*标识符的种类*}
  types = (notyp, ints, reals, bools, chars, arrays, records); {*标识符的类型*}
  symset = set of symbol; {*单词符号类型的集合*}
  typset = set of types; {*标识符类型的集合*}

  item = record  {*记录元素的类型和索引*}
    typ: types;
    ref: index;
  end;

  order = packed record  {*P代码表元素类型*}
    f: -omax.. +omax;
    x: -lmax.. +lmax;
    y: -nmax.. +nmax
  end;
var
  ch: char; {* 从源程序中读取的上一个字符 *}
  rnum: real; {* 记录insymbol过程读到的实数的实数部分 *}
  inum: integer;     {* 记录insymbol过程读到的整数 *}
  sleng: integer;     {* 记录字符串长度 *}
  cc: integer;     {*字符计数器，记录当前行已经读到的字符个数*}
  lc: integer;     {*P代码表索引，始终指向下一条将生成的指令在code表中的位置*}{* program location counter *}
  ll: integer;     {*记录当前行长度*}
  errpos: integer;  {*错误的位置*}
  t, a, b, sx, c1, c2: integer;  {*t:符号表索引；a:数组信息向量表索引；b:分程序表索引；sx：字符串常量表索引；c2:实常量表索引*}{* indices to tables *}
  iflag, oflag, skipflag, stackdump, prtables: boolean; {*各种标志位*}
  sy: symbol;      {*insymbol读到的上一个符号的类型*}
  errs: set of 0..ermax; {*错误编号集合*}
  id: alfa;        {* 记录insymbol过程中标识符名字的字符数组 *}
  progname: alfa;  {*记录程序名*}
  stantyps: typset; {*基本(标准)数据类型集合*}
  constbegsys, typebegsys, blockbegsys, facbegsys, statbegsys: symset;
  {*常量初始符号集合、类型初始符号集合、分程序初始符号集合、因子初始符号集合、语句初始符号集合*}

  line: array[1..llng] of char; {*保存读入的一行代码*}
  key: array[1..nkw] of alfa; {*保存保留字的数组，元素类型为字符串*}
  ksy: array[1..nkw] of symbol; {*保存保留字对应的枚举类型symbol*}
  sps: array[char] of symbol;  {*特殊符号数组sps*}
  display: array[0..lmax] of integer; {*分程序索引表，索引为level，表项内容为指向btab表的指针 *}

  tab: array[0..tmax] of {*符号表，索引为t*} {* indentifier lable *}
  packed record
    name: alfa; {*标识符名字，取前10个字符*}
    link: index; {*指向同一分程序中上一个标识符在tab表中的位置，过程名或函数名为0*}
    obj: objecttyp; {*标识符种类：常量、变量、类型、过程、函数*}
    typ: types; {*标识符类型：整型、实型、数组型、记录型等等*}
    ref: index; {*数组：指向其在atab表中登录的位置；记录、过程名、函数名：指向其在btab表中的指针值*}
    normal: boolean; {*false：标识符为变量形参；true：值形参或其他变量，标准名；*}
    lev: 0..lmax; {*标识符所在分程序的静态层次，主程序为1*}
    adr: integer {*变量、形参：在运行栈S中的相对地址；记录域名：相对记录变量起始地址的位移量；过程名、函数名：相应目标代码的入口地址；整型常量：对应数值；实常量：实常量表中的位置；类型名：大小*}
  end;

  atab: array[1..amax] of  {*数组信息向量表，记录数组详细信息，索引为a*}  {* array-table *}
  packed record
    inxtyp, eltyp: types;
    elref, low, high, elsize, size: index
    {*
      inxtyp：下标（索引）类型．可以是ints、bools、chars；
      eltyp：数组元素类型，可为任意类型；
      elref：元素为数组：指向该元素数组在atab表中登记项的位置；元素为记录：指向该元索记录在at8b表中登录的位置；其他悄况为0；
      low，high：该维数组的上、下界值；
      elsize，size：分别为数组元素的大小和该数组的大小即它们占有的存储单元数
    *}
  end;

  btab: array[1..bmax] of  {*分程序表，索引为b*}  {* block-table *}
  packed record
    last, lastpar, psize, vsize: index
    {*
      last指向分程序当前最后一个标识符在tab表中的位置；
      lastpar指向过程或函数的最后一个参数在tab表中的位置；
      psize为参数及该分程序在运行栈S中的内务信息区所占的存储单元数；
      vsize局部变量、参数及内务信息区在S栈中所占的存储单元总数。
      编译记录类型时，last为当前域名在tab表中的位置，vsize为整个记录的存储单元数，其他为0。
    *}
  end;

  stab: packed array[0..smax] of char; {*字符串常量表，索引为sx*}
  rconst: array[1..c2max] of real; {*实常量表，索引为c2*}
  code: array[0..cmax] of order; {*P代码表，索引为lc*}
  psin, psout, prr, prd: text; {*各个输入、输出、结果输出文件的文件指针*}      {* default in pascal p *}
  inf, outf, fprr: string; {*输入、输出、结果输出的文件路径*}

  procedure errormsg; {*打印程序中出错信息的摘要*}
  var
    k: integer;
    msg: array[0..ermax] of alfa; {*msg数组记录错误信息*}

    begin {*初始化msg数组，记录各种错误*}
    msg[0] := 'undef id  ';
    msg[1] := 'multi def ';
    msg[2] := 'identifier';
    msg[3] := 'program   ';
    msg[4] := ')         ';
    msg[5] := ':         ';
    msg[6] := 'syntax    ';
    msg[7] := 'ident,var ';
    msg[8] := 'of        ';
    msg[9] := '(         ';
    msg[10] := 'id,array  ';
    msg[11] := '(         ';
    msg[12] := ']         ';
    msg[13] := '..        ';
    msg[14] := ';         ';
    msg[15] := 'func. type';
    msg[16] := '=         ';
    msg[17] := 'boolean   ';
    msg[18] := 'convar typ';
    msg[19] := 'type      ';
    msg[20] := 'prog.param';
    msg[21] := 'too big   ';
    msg[22] := '.         ';
    msg[23] := 'type(case)';
    msg[24] := 'character ';
    msg[25] := 'const id  ';
    msg[26] := 'index type';
    msg[27] := 'indexbound';
    msg[28] := 'no array  ';
    msg[29] := 'type id   ';
    msg[30] := 'undef type';
    msg[31] := 'no record ';
    msg[32] := 'boole type';
    msg[33] := 'arith type';
    msg[34] := 'integer   ';
    msg[35] := 'types     ';
    msg[36] := 'param type';
    msg[37] := 'variab id ';
    msg[38] := 'string    ';
    msg[39] := 'no.of pars';
    msg[40] := 'real numbr';
    msg[41] := 'type      ';
    msg[42] := 'real type ';
    msg[43] := 'integer   ';
    msg[44] := 'var,const ';
    msg[45] := 'var,proc  ';
    msg[46] := 'types(:=) ';
    msg[47] := 'typ(case) ';
    msg[48] := 'type      ';
    msg[49] := 'store ovfl';
    msg[50] := 'constant  ';
    msg[51] := ':=        ';
    msg[52] := 'then      ';
    msg[53] := 'until     ';
    msg[54] := 'do        ';
    msg[55] := 'to downto ';
    msg[56] := 'begin     ';
    msg[57] := 'end       ';
    msg[58] := 'factor';

    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout, 'key words'); {*在编译输出文件中输出提示信息并换行*}
    k := 0;
    while errs <> [] do  {*错误编号集合不为空时*}
    begin
      while not (k in errs) do
        k := k + 1;   {*查找集合现有中的错误编号*}
      writeln(psout, k, ' ', msg[k]); {*在编译输出文件中输出错误编号及错误信息摘要*}
      errs := errs - [k];  {*从错误编号集合中移除该编号*}
    end; {* while errs *}
  end {* errormsg *};

  procedure endskip; {*源程序出错后在被跳读的部分下面印出下划线标志*}
  begin                 {* underline skipped part of input *}
    while errpos < cc do
    begin {*当错误位置小于当前行已读字符时，输出下划线*}
      write(psout, '-');
      errpos := errpos + 1;
    end;
    skipflag := false;  {*跳读flag置为false*}
  end {* endskip *};


  procedure nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
  begin
    if cc = ll then{*当前行字符计数器等于当前行长度*}
    begin
      if eof(psin) then{*读到文件尾*}
      begin
        writeln(psout); {*在编译输出文件中输出空行*}{*输出空行*}
        writeln(psout, 'program incomplete');
        errormsg; {*打印程序中出错信息的摘要*}
        exit;  {*退出nextch子过程*}
      end;
      if errpos <> 0 then {*错误位置指针不为0时，表明存在错误*}
      begin
        if skipflag then {*跳读标志为真时*}
          endskip; {*在被跳读的部分下面印出下划线标志*}
        writeln(psout); {*在编译输出文件中输出空行*}
        errpos := 0; {*重置错误位置指针*}
      end;
      write(psout, lc: 5, ' '); {*在编译输出文件中输出P代码表索引*}
      ll := 0; {*重置当前行长度*}
      cc := 0; {*重置当前行字符计数器*}
      while not eoln(psin) do {*没有读到行尾时*}
      begin {*循环读取、输出字符，并保存在line数组中，并统计字符数*}
        ll := ll + 1;
        read(psin, ch);
        write(psout, ch);
        line[ll] := ch;
      end;
      ll := ll + 1;
      readln(psin); {*换行*}
      line[ll] := ' '; {*行末置空格*}
      writeln(psout); {*在编译输出文件中输出空行*}
    end;
    cc := cc + 1;
    ch := line[cc]; {*在line数组中向后读取一个字符*}
  end {* nextch *};

  procedure error(n: integer); {*打印出错位置和出错编号*}
  begin
    if errpos = 0 then
      write(psout, '****'); {*错误位置为0时输出****}
    if cc > errpos then
    begin
      write(psout, ' ': cc - errpos, '^', n: 2); {*打印出错的位置和对应的错误编号*}
      errpos := cc + 3; {*？？？*}
      errs := errs + [n]; {*将编号n加入错误编号集合*}
    end;
  end {* error *};

  procedure fatal(n: integer); {*打印表格溢出信息*}
  var
    msg: array[1..7] of alfa; {*msg数组记录错误信息*}
  begin
    writeln(psout); {*在编译输出文件中输出空行*}
    errormsg; {*打印程序中出错信息的摘要*}
    msg[1] := 'identifier';
    msg[2] := 'procedures';
    msg[3] := 'reals     ';
    msg[4] := 'arrays    ';
    msg[5] := 'levels    ';
    msg[6] := 'code      ';
    msg[7] := 'strings   ';
    writeln(psout, 'compiler table for ', msg[n], ' is too small'); {*输出溢出表的信息*}
    exit; {*终止编译*}
  end {* fatal *};

  procedure insymbol; {*读取下一单词符号，处理注释行*}
  label
    1, 2, 3; {*设置标签*}
  var
    i, j, k, e: integer;

    procedure readscale; {*处理实数的指数部分*}
    var
      s, sign: integer; {*sign记录指数的符号位，s记录数值*}
    begin
      nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
      sign := 1; {*符号位初始化为1*}
      s := 0;
      if ch = '+' then
      else if ch = '-' then
      begin
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        sign := -1;
      end;
      if not ((ch >= '0') and (ch <= '9')) then
        error(40) {*当前字符不为数字时，打印出错位置和出错编号*}
      else
        repeat {*循环更新s的值直到读入非数字字符*}
          s := 10 * s + ord(ord(ch) - ord('0'));
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        until not ((ch >= '0') and (ch <= '9'));
      e := s * sign + e; {*计算最终的指数值*}
    end {* readscale *};

    procedure adjustscale; {*根据小数位数和指数大小求出实数数值*}
    var
      s: integer;
      d, t: real;
    begin
      if k + e > emax then
        error(21) {*实数的整数位数与指数之和超过最大值时报错*}
      else if k + e < emin then
        rnum := 0  {*实数的整数位数与指数之和低于最小值时将实数置为0*}
      else
      begin
        s := abs(e); {*计算指数的绝对值*}
        t := 1.0;
        d := 10.0;
        repeat {*通过循环计算10的s次幂的数值，存在d中*}
          while not odd(s) do {*指数不为奇数时*}
          begin
            s := s div 2;
            d := sqr(d);
          end;
          s := s - 1;
          t := d * t
        until s = 0;
        if e >= 0 then {*计算最终的实数的大小,分指数正负*}
          rnum := rnum * t
        else
          rnum := rnum / t;
      end;
    end {* adjustscale *};

    procedure options;  {*处理编译时的可选项*}

      procedure switch(var b: boolean); {*处理编译可选项中的‘+’‘-’标志*}
      begin
        b := ch = '+'; {*读入字符为+时b为true，否则b为false*}
        if not b then
          if not (ch = '-') then {*读入的符号不是+或-时，输出错误信息*}
          begin {* print error message *}
            while (ch <> '*') and (ch <> ',') do
              nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
          end
          else
            nextch {*读取下一字符，处理行结束符，印出被编译的源程序*}
        else
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
      end {* switch *};

    begin {* options  *}
      repeat
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        if ch <> '*' then {*在$后继续读取到*时*}
        begin
          if ch = 't' then {*读入t后需要判断编译后是否打印表格内容*}
          begin
            nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
            switch(prtables); {*处理编译可选项中的‘+’‘-’标志*}
          end
          else if ch = 's' then {*读入s后需要判断运行出错时是否进行现场卸出打印*}
          begin
            nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
            switch(stackdump); {*处理编译可选项中的‘+’‘-’标志*}
          end;

        end
      until ch <> ','; {*读取下一字符不为逗号时循环终止*}
    end {* options *};

  begin {* insymbol  *}
    1:
      while (ch = ' ') or (ch = chr(9)) do {*跳过所有的空格和制表符*}
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}    {* space & htab *}
    case ch of
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
      'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
      's', 't', 'u', 'v', 'w', 'x', 'y', 'z':
      begin {*当前读入字符为字母时*}{* identifier of wordsymbol *}
        k := 0;
        id := '          ';
        repeat {*用一个循环读入一个单词，k记录单词长度*}
          if k < alng then
          begin {*当前单词长度小于标识符最大长度时不断读入字符并保存在id数组*}
            k := k + 1;
            id[k] := ch;
          end;
          nextch
        until not (((ch >= 'a') and (ch <= 'z')) or
            ((ch >= '0') and (ch <= '9')));
        i := 1;
        j := nkw; {*保留字的个数*}{* binary search *}
        repeat {*通过二分查找，判断该单词是否为保留字*}
          k := (i + j) div 2;
          if id <= key[k] then
            j := k - 1;
          if id >= key[k] then
            i := k + 1;
        until i > j;
        if i - 1 > j then
          sy := ksy[k] {*找到说明该单词为保留字*}
        else
          sy := ident; {*未找到说明该单词为标识符*}
      end;
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9':
      begin {*当前读入字符为数字时*}{* number *}
        k := 0;
        inum := 0;
        sy := intcon; {*符号类型记为整数*}
        repeat  {*循环读入当前数的整数部分，直到遇到非数字*}
          inum := inum * 10 + ord(ch) - ord('0');
          k := k + 1; {*k记录整数的位数*}
          nextch
        until not ((ch >= '0') and (ch <= '9'));
        if (k > kmax) or (inum > nmax) then
        begin {*位数超范围或整数大小超范围时,报错并重置inum，k*}
          error(21);
          inum := 0;
          k := 0;
        end;
        if ch = '.' then {*读到小数点时*}
        begin
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
          if ch = '.' then
            ch := ':' {*连续读到2个'.'相当于读到':'*}
          else
          begin
            sy := realcon; {*符号类型记为实数*}
            rnum := inum; {*初始化底数和指数*}
            e := 0;
            while (ch >= '0') and (ch <= '9') do
            begin
              e := e - 1; {*小数点后每多一位指数减1*}
              rnum := 10.0 * rnum + (ord(ch) - ord('0'));
              nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
            end;
            if e = 0 then {*小数点后没有数字时报错*}
              error(40);
            if ch = 'e' then {*读入e表明需要继续读入指数部分*}
              readscale; {*读入指数部分*}
            if e <> 0 then
              adjustscale; {*指数不为0时根据小数位数和指数大小求出实数数值*}
          end;
        end
        else if ch = 'e' then {*读入e表明需要继续读入指数部分*}
        begin
          sy := realcon; {*符号类型记为实数*}
          rnum := inum; {*初始化底数和指数*}
          e := 0;
          readscale; {*读入指数部分*}
          if e <> 0 then
            adjustscale; {*指数不为0时根据小数位数和指数大小求出实数数值*}
        end;
      end;
      ':':
      begin {*当前读入字符为':'时*}
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        if ch = '=' then
        begin {*后接等号时说明为赋值符号*}
          sy := becomes; {*符号类型记为赋值符号*}
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        end
        else  {*否则符号类型记为冒号*}
          sy := colon;
      end;
      '<':
      begin {*当前读入字符为小于号时*}
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        if ch = '=' then
        begin  {*后接等号时符号类型记为小于等于号*}
          sy := leq;
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        end
        else
        if ch = '>' then
        begin  {*后接大于号时符号类型记为不等号*}
          sy := neq;
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        end
        else {*否则该符号类型记为小于号*}
          sy := lss;
      end;
      '>':
      begin {*当前读入字符为大于于号时*}
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        if ch = '=' then
        begin {*后接等号时符号类型记为大于等于号*}
          sy := geq;
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        end
        else  {*否则该符号类型记为大于号*}
          sy := gtr;
      end;
      '.':
      begin {*当前读入字符为'.'时*}
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        if ch = '.' then
        begin {*连续读入两个'.'时符号类型记为冒号*}
          sy := colon;
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        end
        else  {*否则该符号类型记为点号*}
          sy := period;
      end;
      '''':
      begin {*当前读入字符为单引号时*}
        k := 0;
        2:
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        if ch = '''' then  {*下一个字符还是单引号时*}
        begin
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
          if ch <> '''' then  {*下一字符不是单引号时，字符串读入结束，跳至3判断*}
            goto 3;
        end;
        if sx + k = smax then {*字符串常量表溢出时报错*}
          fatal(7);
        stab[sx + k] := ch; {*未溢出时将字符存入表中，k记录存入的字符个数*}
        k := k + 1;
        if cc = 1 then {*当前行已经读到的字符个数为1时，已经读到行尾，k的值置为0*}
        begin {* end of line *}
          k := 0; {*k的值置为0*}
        end
        else  {*否则跳转到2继续读入字符*}
          goto 2;
        3:
          if k = 1 then {*字符串只含一个字符*}
          begin
            sy := charcon; {*该符号类型记为字符类型，inum保存字符的ASCII值*}
            inum := ord(stab[sx]);
          end
          else if k = 0 then
          begin  {*字符串为空时，报错并将符号类型记为字符类型，重置inum*}
            error(38);
            sy := charcon;
            inum := 0;
          end
          else
          begin {*字符串长度大于1时*}
            sy := stringcon; {*对应符号类型记为字符串类型*}
            inum := sx; {*inum记录字符串在字符串常量表中的索引sx*}
            sleng := k; {*sleng记录字符串长度k*}
            sx := sx + k; {*更新字符串常量表索引sx*}
          end;
      end;
      '(':
      begin {*当前读入字符为左括号时*}
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        if ch <> '*' then
          sy := lparent {*下一字符不为*时，将符号类型记为左括号*}
        else
        begin {*否则读取到的内容为注释*}
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
          if ch = '$' then
            options;  {*处理编译时的可选项*}
          repeat {*循环读取字符并忽略，直到读到右括号为止*}
            while ch <> '*' do {*循环读取字符并忽略，直到读到*字符*}
              nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
            nextch
          until ch = ')';
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
          goto 1; {*读取完注释后跳到insymbol开头重新开始读取字符*}
        end;
      end;
      '{':
      begin  {*当前读入字符为左大括号时*}
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        if ch = '$' then
          options;  {*读到$字符时，处理编译时的可选项*}
        while ch <> '}' do  {*循环读取字符并忽略，直到读到右括号为止*}
          nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        goto 1;  {*读取完注释后重新开始读取字符*}
      end;
      '+', '-', '*', '/', ')', '=', ',', '[', ']', ';':
      begin
        sy := sps[ch];
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
      end;
      '$', '"', '@', '?', '&', '^', '!':
      begin
        error(24);
        nextch; {*读取下一字符，处理行结束符，印出被编译的源程序*}
        goto 1;
      end
    end; {* case *}
  end {* insymbol *};

  procedure enter(x0: alfa; x1: objecttyp; x2: types; x3: integer);
  begin {*把标准类型、过程和函数的名字登录到符号表tab中*}
    t := t + 1;    {*更新符号表索引*}
    with tab[t] do  {*在符号表中登记各项信息*}
    begin
      name := x0;
      link := t - 1;
      obj := x1;
      typ := x2;
      ref := 0; {*对于那些基本数据类型ref默认为0*}
      normal := true; {*标准名normal域为true*}
      lev := 0;
      adr := x3;
    end;
  end; {* enter *}

  procedure enterarray(tp: types; l, h: integer);
  begin {*登录数组信息向量表*}
    if l > h then
      error(27); {*下界大于上界时报错*}
    if (abs(l) > xmax) or (abs(h) > xmax) then
    begin
      error(27); {*上下界超范围时报错*}
      l := 0;
      h := 0;
    end;
    if a = amax then
      fatal(4) {*数组信息向量表atab表溢出报错*}
    else
    begin
      a := a + 1;
      with atab[a] do
      begin  {*登录数组信息向量表，记录数组类型，上下界*}
        inxtyp := tp;
        low := l;
        high := h;
      end;
    end;
  end {* enterarray *};

  procedure enterblock; {*登录分程序表btab*}
  begin
    if b = bmax then
      fatal(2) {*分程序表溢出时报错*}
    else
    begin {*否则登录分程序表btab*}
      b := b + 1;
      btab[b].last := 0;
      btab[b].lastpar := 0;
    end;
  end {* enterblock *};

  procedure enterreal(x: real); {*登录实常数表（rconso表）*}
  begin
    if c2 = c2max - 1 then
      fatal(3) {*实常数表溢出时报错*}
    else
    begin
      rconst[c2 + 1] := x;
      c1 := 1;
      while rconst[c1] <> x do {*在实常量表中查询是否已经存在该常量*}
        c1 := c1 + 1;
      if c1 > c2 then {*不存在该常量（c1 > c2）时更新索引c2，存入该实常量*}
        c2 := c1;
    end;
  end {* enterreal *};

  procedure emit(fct: integer); {*生成P代码指令*}
  begin
    if lc = cmax then
      fatal(6); {*P代码表溢出报错*}
    code[lc].f := fct; {*生成P代码指令*}
    lc := lc + 1;
  end {* emit *};


  procedure emit1(fct, b: integer); {*生成P代码指令*}
  begin
    if lc = cmax then
      fatal(6); {*P代码表溢出报错*}
    with code[lc] do
    begin  {*生成P代码指令*}
      f := fct;
      y := b;
    end;
    lc := lc + 1;
  end {* emit1 *};

  procedure emit2(fct, a, b: integer); {*生成p代码指令*}
  begin
    if lc = cmax then
      fatal(6);  {*P代码表溢出报错*}
    with code[lc] do
    begin {*生成P代码指令*}
      f := fct;
      x := a;
      y := b;
    end;
    lc := lc + 1;
  end {* emit2 *};

  procedure printtables; {*打印编译生成的符号表、分程序表、实常数表以及p代码*}
  var
    i: integer;
    o: order;
    mne: array[0..omax] of packed array[1..5] of char;
  begin
    mne[0] := 'LDA  '; {*初始化指令表*}
    mne[1] := 'LOD  ';
    mne[2] := 'LDI  ';
    mne[3] := 'DIS  ';
    mne[8] := 'FCT  ';
    mne[9] := 'INT  ';
    mne[10] := 'JMP  ';
    mne[11] := 'JPC  ';
    mne[12] := 'SWT  ';
    mne[13] := 'CAS  ';
    mne[14] := 'F1U  ';
    mne[15] := 'F2U  ';
    mne[16] := 'F1D  ';
    mne[17] := 'F2D  ';
    mne[18] := 'MKS  ';
    mne[19] := 'CAL  ';
    mne[20] := 'IDX  ';
    mne[21] := 'IXX  ';
    mne[22] := 'LDB  ';
    mne[23] := 'CPB  ';
    mne[24] := 'LDC  ';
    mne[25] := 'LDR  ';
    mne[26] := 'FLT  ';
    mne[27] := 'RED  ';
    mne[28] := 'WRS  ';
    mne[29] := 'WRW  ';
    mne[30] := 'WRU  ';
    mne[31] := 'HLT  ';
    mne[32] := 'EXP  ';
    mne[33] := 'EXF  ';
    mne[34] := 'LDT  ';
    mne[35] := 'NOT  ';
    mne[36] := 'MUS  ';
    mne[37] := 'WRR  ';
    mne[38] := 'STO  ';
    mne[39] := 'EQR  ';
    mne[40] := 'NER  ';
    mne[41] := 'LSR  ';
    mne[42] := 'LER  ';
    mne[43] := 'GTR  ';
    mne[44] := 'GER  ';
    mne[45] := 'EQL  ';
    mne[46] := 'NEQ  ';
    mne[47] := 'LSS  ';
    mne[48] := 'LEQ  ';
    mne[49] := 'GRT  ';
    mne[50] := 'GEQ  ';
    mne[51] := 'ORR  ';
    mne[52] := 'ADD  ';
    mne[53] := 'SUB  ';
    mne[54] := 'ADR  ';
    mne[55] := 'SUR  ';
    mne[56] := 'AND  ';
    mne[57] := 'MUL  ';
    mne[58] := 'DIV  ';
    mne[59] := 'MOD  ';
    mne[60] := 'MUR  ';
    mne[61] := 'DIR  ';
    mne[62] := 'RDL  ';
    mne[63] := 'WRL  ';

    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout, '   identifiers  link  obj  typ  ref  nrm  lev  adr'); {*输出提示信息*}
    writeln(psout); {*在编译输出文件中输出空行*}
    for i := btab[1].last to t do {*打印符号表*}
      with tab[i] do
        writeln(psout, i, ' ', name, link: 5, ord(obj): 5, ord(typ): 5, ref: 5,
          ord(normal): 5, lev: 5, adr: 5);
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout, 'blocks   last  lpar  psze  vsze');
    writeln(psout); {*在编译输出文件中输出空行*}
    for i := 1 to b do {*打印分程序表*}
      with btab[i] do
        writeln(psout, i: 4, last: 9, lastpar: 5, psize: 5, vsize: 5);
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout, 'arrays xtyp etyp eref low high elsz size');
    writeln(psout); {*在编译输出文件中输出空行*}
    for i := 1 to a do
      with atab[i] do  {*打印数组信息向量表*}
        writeln(psout, i: 4, ord(inxtyp): 9, ord(eltyp): 5, elref: 5,
          low: 5, high: 5, elsize: 5, size: 5);
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout, 'code:');
    writeln(psout); {*在编译输出文件中输出空行*}
    for i := 0 to lc - 1 do{*打印Pcode表*}
    begin
      write(psout, i: 5);
      o := code[i];
      write(psout, mne[o.f]: 8, o.f: 5);
      if o.f < 31 then
        if o.f < 4 then
          write(psout, o.x: 5, o.y: 5)
        else
          write(psout, o.y: 10)
      else
        write(psout, '          ');
      writeln(psout, ',');
    end;
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout, 'Starting address is ', tab[btab[1].last].adr: 5); {*打印程序入口地址*}
  end {* printtables *};


  procedure block(fsys: symset; isfun: boolean; level: integer);
  type {*分析处理分程序*}
    conrec = record  {*结构体根据不同的type类型来保存不同样式的数据，用来保存常量的信息*}
      case tp: types of
        ints, chars, bools: (i: integer);
        reals: (r: real)
    end;
  var
    dx : integer ; {*数据存储分配地址的索引*}{* data allocation index *}
    prt: integer;  {*当前过程的符号表索引*}{* t-index of this procedure *}
    prb: integer;  {*当前过程的btab表索引*}{* b-index of this procedure *}
    x: integer;

    procedure skip(fsys: symset; n: integer);
    begin {*跳读源程序，直至取来的符号属于给出的符号集为止，并打印出错标志*}
      error(n); {*先进行报错*}
      skipflag := true; {*跳读标志置位*}
      while not (sy in fsys) do
        insymbol; {*读取下一单词符号，处理注释行*}
      if skipflag then
        endskip; {*源程序出错后在被跳读的部分下面印出下划线标志*}
    end {* skip *};

    procedure test(s1, s2: symset; n: integer);
    {*测试当前符号是否合法，若不合法,打印出错标志并进行跳读*}
    begin
      if not (sy in s1) then
        skip(s1 + s2, n); {*当前符号不合法时跳读s1 + s2符号集中的符号*}
    end {* test *};

    procedure testsemicolon; {*测试当前符号是否为分号*}
    begin
      if sy = semicolon then
        insymbol {*当前为分号时继续读取下一符号*}
      else
      begin
        error(14); {*否则报错*}
        if sy in [comma, colon] then {*若当前符号为逗号或冒号*}
          insymbol; {*读取下一单词符号，处理注释行*}
      end;
      test([ident] + blockbegsys, fsys, 6); {*测试当前符号是否为标识符或分程序开始符号，否则跳读*}
    end {* testsemicolon *};


    procedure enter(id: alfa; k: objecttyp);
    var {*在符号表中登录分程序说明部分出现的名字*}
      j, l: integer;
    begin
      if t = tmax then
        fatal(1){*符号表溢出时报错*}
      else
      begin
        tab[0].name := id; {*方便在符号表中查找该符号*}
        j := btab[display[level]].last;
        l := j; {*获取当前分程序在tab表中最后一个标识符的位置*}
        while tab[j].name <> id do
          j := tab[j].link; {*在当前分程序的标识符中查找名为id的标识符*}
        if j <> 0 then
          error(1) {*j不为0时说明标识符重复定义，报错*}
        else
        begin
          t := t + 1; {*在符号表中登录各项信息*}
          with tab[t] do
          begin
            name := id;
            link := l;
            obj := k;
            typ := notyp;
            ref := 0;
            lev := level;
            adr := 0;
            normal := false; {*enter过程将normal初始化为false*}
          end;
          btab[display[level]].last := t; {*更新当前分程序在tab表中最后一个标识符的位置*}
        end;
      end;
    end {* enter *};

    function loc(id: alfa): integer;
    var {*查找标识符在符号表中的位置*}
      i, j: integer;        {* locate if in table *}
    begin
      i := level;
      tab[0].name := id;  {* sentinel *}
      repeat {*从当前分程序所在的静态层次开始向外逐层查找*}
        j := btab[display[i]].last;
        while tab[j].name <> id do
          j := tab[j].link;
        i := i - 1;
      until (i < 0) or (j <> 0);
      if j = 0 then
        error(0); {*最终未找到时报错*}
      loc := j; {*返回该符号在tab表的位置，0为不在表中*}
    end {* loc *};

    procedure entervariable; {*登记当前变量到符号表*}
    begin
      if sy = ident then
      begin {*当前符号为标识符时进行登记，否则报错*}
        enter(id, vvariable); {*在符号表中登录分程序说明部分出现的名字*}
        insymbol; {*读取下一单词符号，处理注释行*}
      end
      else
        error(2);
    end {* entervariable *};

    procedure constant(fsys: symset; var c: conrec);
    var {*处理程序中出现的常量，并由参数c返回该常量的类型和数值*}
      x, sign: integer; {*sign为符号位*}
    begin
      c.tp := notyp; {*常量类型默认为notyp*}
      c.i := 0;
      test(constbegsys, fsys, 50); {*测试当前符号是否为常量开始符号*}
      if sy in constbegsys then
      begin
        if sy = charcon then {*当前符号类型为char时*}
        begin
          c.tp := chars;
          c.i := inum; {*inum为该字符的ASCII码*}
          insymbol; {*读取下一单词符号，处理注释行*}
        end
        else
        begin
          sign := 1;
          if sy in [plus, minus] then {*读到正负号时*}
          begin
            if sy = minus then
              sign := -1; {*读到负号时设置符号位*}
            insymbol; {*读取下一单词符号，处理注释行*}
          end;
          if sy = ident then {*当前符号为标识符时*}
          begin
            x := loc(id); {*查找标识符在符号表中的位置*}
            if x <> 0 then {*在符号表中查到该符号*}
              if tab[x].obj <> konstant then
                error(25) {*符号种类不为常量时报错*}
              else
              begin {*符号种类为常量时获得该常量的类型和数值*}
                c.tp := tab[x].typ;
                if c.tp = reals then
                  c.r := sign * rconst[tab[x].adr] {*实常量的数值要查表获得*}
                else
                  c.i := sign * tab[x].adr;
              end;
            insymbol; {*读取下一单词符号，处理注释行*}
          end
          else if sy = intcon then {*当前符号类型为int时*}
          begin
            c.tp := ints;
            c.i := sign * inum;
            insymbol; {*读取下一单词符号，处理注释行*}
          end
          else if sy = realcon then {*当前符号类型为real时*}
          begin
            c.tp := reals;
            c.r := sign * rnum;
            insymbol; {*读取下一单词符号，处理注释行*}
          end
          else {*否则跳读源程序，直至取来的符号属于给出的符号集fsys为止，并打印出错标志*}
            skip(fsys, 50);
        end;
        test(fsys, [], 6); {*测试符号是否合法*}
      end;
    end {* constant *};

    procedure typ(fsys: symset; var tp: types; var rf, sz: integer);
    var {*处理类型描述，由参数得到它的类型tp，指向类型详细信息表的指针ref和该类型大小sz*}
      eltp: types; {*记录元素的类型*}
      elrf, x: integer; {*elrf记录元素的指针*}
      elsz, offset, t0, t1: integer; {*elsz记录元素的大小*}

      procedure arraytyp(var aref, arsz: integer);
      var {*处理数组类型，由参数返回指向该数组信息向量表的指针aref和数组大小arsz*}
        eltp: types; {*记录数组元素类型*}
        low, high: conrec; {*记录数组上下界*}
        elrf, elsz: integer; {*记录指向数组元素的指针和元素大小*}
      begin
        constant([colon, rbrack, rparent, ofsy] + fsys, low);
        {*处理程序中出现的常量，并由参数low返回数组下界的类型和数值*}
        if low.tp = reals then
        begin {*数组下界为实数时报错并将下界置为0*}
          error(27);
          low.tp := ints;
          low.i := 0;
        end;
        if sy = colon then {*下界后面跟着冒号时*}
          insymbol
        else
          error(13);
        constant([rbrack, comma, rparent, ofsy] + fsys, high);
        {*处理程序中出现的常量，并由参数high返回数组上界的类型和数值*}
        if high.tp <> low.tp then
        begin  {*上界类型不是整型时报错并将下界赋值给上界*}
          error(27);
          high.i := low.i;
        end;
        enterarray(low.tp, low.i, high.i); {*登录数组信息向量表*}
        aref := a; {*获取数组在atab表中登录的位置*}
        if sy = comma then {*读到逗号，说明为多维数组*}
        begin
          insymbol; {*读取下一单词符号，处理注释行*}
          eltp := arrays; {*元素类型设置为数组*}
          arraytyp(elrf, elsz); {*递归处理数组类型，由参数返回指向该数组信息向量表的指针aref和数组大小arsz*}
        end
        else
        begin
          if sy = rbrack then {*读到‘]’时*}
            insymbol {*读取下一单词符号，处理注释行*}
          else
          begin {*未读到‘]’时报错*}
            error(12);
            if sy = rparent then
              insymbol; {*读取下一单词符号，处理注释行*}
          end;
          if sy = ofsy then {*读到保留字of时*}
            insymbol {*读取下一单词符号，处理注释行*}
          else
            error(8);
          typ(fsys, eltp, elrf, elsz);
          {*递归调用typ处理类型描述，由参数得到元素的类型eltp，指向元素类型详细信息表的指针elrf和元素类型大小sz*}
        end;
        with atab[aref] do
        begin {*在数组信息向量表中记录当前数组的详细信息*}
          arsz := (high - low + 1) * elsz;
          size := arsz;
          eltyp := eltp;
          elref := elrf;
          elsize := elsz;
        end;
      end {* arraytyp *};

    begin {* typ  *}
      tp := notyp; {*初始化类型信息*}
      rf := 0;
      sz := 0;
      test(typebegsys, fsys, 10); {*测试当前符号是否为类型声明开始符号*}
      if sy in typebegsys then {*如果当前符号是类型声明开始符号（ident, arraysy, recordsy）*}
      begin
        if sy = ident then
        begin
          x := loc(id); {*查找标识符在符号表中的位置*}
          if x <> 0 then
            with tab[x] do
              if obj <> typel then
                error(29) {*查到的标识符种类不为类型时报错*}
              else
              begin {*获得该标识符在符号表登记各项信息*}
                tp := typ;
                rf := ref;
                sz := adr;
                if tp = notyp then
                  error(30); {*该标识符类型为notyp时报错*}
              end;
          insymbol; {*读取下一单词符号，处理注释行*}
        end
        else if sy = arraysy then {*当前符号为数组声明保留字array*}
        begin
          insymbol; {*读取下一单词符号，处理注释行*}
          if sy = lbrack then {*当前读到的符号为'['时*}
            insymbol
          else
          begin
            error(11); {*当前读到的符号不为为'['时报错*}
            if sy = lparent then {*当前读到的符号为'('时容错*}
              insymbol; {*读取下一单词符号，处理注释行*}
          end;
          tp := arrays; {*类型设置为数组*}
          arraytyp(rf, sz);
          {*处理数组类型，由参数返回指向该数组信息向量表的指针aref和数组大小arsz*}
        end
        else  {*此时当前符号类型只可能是recordsy*}
        begin {* records *}
          insymbol; {*读取下一单词符号，处理注释行*}
          enterblock; {*登录分程序表btab*}
          tp := records;  {*类型设置为记录*}
          rf := b; {*rf为记录在分程序表btab中的位置*}
          if level = lmax then{*display表溢出报错*}
            fatal(5);
          level := level + 1;
          display[level] := b; {*登录分程序索引表*}
          offset := 0;
          while not (sy in fsys - [semicolon, comma, ident] + [endsy]) do{*读到分号，逗号或标识符时循环？*}
          begin {*依次处理记录内部的每个成员变量*}{* field section *}
            if sy = ident then
            begin
              t0 := t; {*获得当前tab表的索引*}
              entervariable; {*登记当前变量到符号表*}
              while sy = comma do
              begin
                insymbol; {*读取下一单词符号，处理注释行*}
                entervariable; {*登记当前变量到符号表*}
              end;
              if sy = colon then
                insymbol
              else
                error(5); {*未读到冒号时报错*}
              t1 := t; {*获得当前tab表的索引*}
              typ(fsys + [semicolon, endsy, comma, ident],
                eltp, elrf, elsz);
              {*处理类型描述，由参数得到它的元素类型eltp，指向元素类型详细信息表的指针elrf和该元素类型大小elsz*}
              while t0 < t1 do
              begin {*用获得的信息回填符号表t0到t1之间成员变量缺少的信息*}
                t0 := t0 + 1;
                with tab[t0] do
                begin
                  typ := eltp;
                  ref := elrf;
                  normal := true;
                  adr := offset; {*adr填入该成员变量相对记录初始地址的偏移量*}
                  offset := offset + elsz; {*更新偏移量，加元素大小*}
                end;
              end;
            end; {* sy = ident *}
            if sy <> endsy then
            begin
              if sy = semicolon then
                insymbol
              else
              begin
                error(14); {*缺少分号时报错*}
                if sy = comma then{*读到逗号时容错*}
                  insymbol; {*读取下一单词符号，处理注释行*}
              end;
              test([ident, endsy, semicolon], fsys, 6); {*测试当前符号是否为标识符，end，分号*}
            end;
          end; {* field section *}
          btab[rf].vsize := offset; {*分程序表填入该记录总大小*}
          sz := offset;
          btab[rf].psize := 0;
          insymbol; {*读取下一单词符号，处理注释行*}
          level := level - 1; {*record声明结束，分程序退回上一层*}
        end; {* record *}
        test(fsys, [], 6);
      end;
    end {* typ *};

    procedure parameterlist;
    {*处理过程或函数说明中的形参表，将形参及其有关信息登录到符号表中*} {* formal parameter list  *}
    var
      tp: types; {*记录当前形参的类型*}
      valpar: boolean;  {*标记当前形参是否为值形参*}
      rf, sz, x, t0: integer; {*记录参数的其他信息*}
    begin
      insymbol; {*读取下一单词符号，处理注释行*}
      tp := notyp;
      rf := 0;
      sz := 0;
      test([ident, varsy], fsys + [rparent], 7); {*测试当前符号是否为标识符或var保留字*}
      while sy in [ident, varsy] do {*当前符号是否为标识符或var保留字时一直循环*}
      begin
        if sy <> varsy then
          valpar := true  {*当前符号不为var时，说明为值形参，valpar设为true*}
        else
        begin  {*否则说明当前形参为变量形参*}
          insymbol; {*读取下一单词符号，处理注释行*}
          valpar := false;
        end;
        t0 := t; {*获取当前符号表索引*}
        entervariable; {*登记当前变量到符号表*}
        while sy = comma do {*有逗号时说明有多个变量*}
        begin
          insymbol; {*读取下一单词符号，处理注释行*}
          entervariable; {*登记当前变量到符号表*}
        end;
        if sy = colon then {*读到冒号时,读入并记录形参类型*}
        begin
          insymbol; {*读取下一单词符号，处理注释行*}
          if sy <> ident then
            error(2) {*下一单词符号不为标识符时报错*}
          else
          begin
            x := loc(id); {*查找标识符在符号表中的位置*}
            insymbol; {*读取下一单词符号，处理注释行*}
            if x <> 0 then {*在tab表中找到该标识符时*}
              with tab[x] do
                if obj <> typel then
                  error(29) {*查找到的符号种类不是类型时报错*}
                else
                begin
                  tp := typ;
                  rf := ref;
                  if valpar then
                    sz := adr {*对于值形参，sz记为该类型符号的adr（类型的大小）*}
                  else
                    sz := 1; {*对于变量形参，sz记为1，保存实参地址*}
                end;
          end;
          test([semicolon, rparent], [comma, ident] + fsys, 14);
          {*测试当前符号是否为分号或右括号*}
        end
        else
          error(5); {*未读到冒号时报错*}
        while t0 < t do {*为每个新存入符号表的形参设置类型和其他信息*}
        begin
          t0 := t0 + 1;
          with tab[t0] do
          begin
            typ := tp;
            ref := rf;
            adr := dx;
            lev := level;
            normal := valpar; {*对于变量形参，normal为false*}
            dx := dx + sz; {*更新运行栈S数据分配地址的索引*}
          end;
        end;
        if sy <> rparent then
        begin
          if sy = semicolon then
            insymbol {*当前符号为分号时继续读入符号*}
          else
          begin
            error(14); {*当前符号不为分号或右括号时报错*}
            if sy = comma then
              insymbol; {*当前符号为逗号读取下一单词符号，处理注释行*}
          end;
          test([ident, varsy], [rparent] + fsys, 6); {*测试当前符号是否为*}
        end;
      end {* while *};
      if sy = rparent then
      begin  {*当前读到的符号为右括号时*}
        insymbol; {*读取下一单词符号，处理注释行*}
        test([semicolon, colon], fsys, 6); {*测试当前符号是否为分号或冒号,否则跳读*}
      end
      else
        error(4);
    end {* parameterlist *};


    procedure constdec; {*处理常量定义，将常量名及其相应信息填人符号表*}
    var
      c: conrec;
    begin
      insymbol; {*读取下一单词符号，处理注释行*}
      test([ident], blockbegsys, 2);
      while sy = ident do {*当前符号为标识符时*}
      begin
        enter(id, konstant); {*在符号表中登录常量名字及种类*}
        insymbol; {*读取下一单词符号，处理注释行*}
        if sy = eql then
          insymbol {*读到等号时继续读符号*}
        else
        begin
          error(16);
          if sy = becomes then {*读到赋值号时容错*}
            insymbol; {*读取下一单词符号，处理注释行*}
        end;
        constant([semicolon, comma, ident] + fsys, c); {*由参数c获得常量的类型和数值*}
        tab[t].typ := c.tp;
        tab[t].ref := 0;
        if c.tp = reals then
        begin  {*常量为实常量时*}
          enterreal(c.r);   {*登录实常数表*}
          tab[t].adr := c1; {*在符号表的adr域保存其实常量表登录位置*}
        end                 {*c1为全局变量，保存该常量在实常量表中登记的位置*}
        else
          tab[t].adr := c.i; {*否则在adr填入常量对应的值*}
        testsemicolon; {*测试当前符号是否为分号*}
      end;
    end {* constdec *};

    procedure typedeclaration; {*处理类型定义，并将类型名及其相应信息填人符号表*}
    var
      tp: types; {*记录类型的类型（ident, arraysy, recordsy三大类）*}
      rf, sz, t1: integer;
    begin
      insymbol; {*读取下一单词符号，处理注释行*}
      test([ident], blockbegsys, 2); {*测试该符号是否为标识符*}
      while sy = ident do {*该符号是标识符时执行循环*}
      begin
        enter(id, typel); {*在符号表中登录该类型的名字*}
        t1 := t; {*获得此类型的名字在符号表的索引*}
        insymbol; {*读取下一单词符号，处理注释行*}
        if sy = eql then
          insymbol  {*如果是等号继续读符号*}
        else
        begin
          error(16);
          if sy = becomes then {*若是赋值符号则进行容错*}
            insymbol; {*读取下一单词符号，处理注释行*}
        end;
        typ([semicolon, comma, ident] + fsys, tp, rf, sz);
        {*处理类型描述，由参数得到它的类型tp，指向类型详细信息表的指针ref和该类型大小sz*}
        with tab[t1] do {*用typ过程获得的信息回填符号表*}
        begin
          typ := tp;
          ref := rf;
          adr := sz;
        end;
        testsemicolon; {*测试当前符号是否为分号*}
      end;
    end {* typedeclaration *};

    procedure variabledeclaration; {*处理变量声明，并将变量名及其相应信息填入符号表*}
    var
      tp: types; {*记录该变量的类型*}
      t0, t1, rf, sz: integer;
    begin
      insymbol; {*读取下一单词符号，处理注释行*}
      while sy = ident do
      begin
        t0 := t; {*获得此时符号表的索引*}
        entervariable; {*登记当前变量到符号表*}
        while sy = comma do {*如果是逗号循环读入并登记多个变量*}
        begin
          insymbol; {*读取下一单词符号，处理注释行*}
          entervariable; {*登记当前变量到符号表*}
        end;
        if sy = colon then
          insymbol
        else
          error(5); {*未读到冒号时报错*}
        t1 := t; {*获得此时符号表的索引*}
        typ([semicolon, comma, ident] + fsys, tp, rf, sz);
        {*处理类型描述，由参数得到它的类型tp，指向类型详细信息表的指针ref和该类型大小sz*}
        while t0 < t1 do {*用typ过程获得的信息回填符号表t0-t1之间的符号*}
        begin
          t0 := t0 + 1;
          with tab[t0] do
          begin
            typ := tp;
            ref := rf;
            lev := level;
            adr := dx; {*dx为变量在运行栈S存储分配地址的索引*}
            normal := true;
            dx := dx + sz;
          end;
        end;
        testsemicolon; {*测试当前符号是否为分号*}
      end;
    end {* variabledeclaration *};

    procedure procdeclaration; {*处理过程或函数说明，将过程、函数名填入符号表。递归调用 block分析处理下一层分程序*}
    var
      isfun: boolean; {*记录标识符是否为函数*}
    begin
      isfun := sy = funcsy;  {*记录标识符是否为函数*}
      insymbol; {*读取下一单词符号，处理注释行*}
      if sy <> ident then
      begin
        error(2); {*符号不为标识符时报错*}
        id := '          '; {*id变量为记录insymbol过程中标识符名字的字符数组 *}
      end;
      if isfun then
        enter(id, funktion) {*在符号表中登录该函数名*}
      else
        enter(id, prozedure); {*在符号表中登录该过程名*}
      tab[t].normal := true; {*因为enter过程将normal初始化为false*}
      insymbol; {*读取下一单词符号，处理注释行*}
      block([semicolon] + fsys, isfun, level + 1); {*递归调用block分析处理下一层分程序*}
      if sy = semicolon then
        insymbol
      else
        error(14); {*处理完成子程序之后当前符号不为分号时报错*}
      emit(32 + ord(isfun)); {*生成P代码指令，退出该过程或函数*}{*exit*}
    end {* proceduredeclaration *};


    procedure statement(fsys: symset); {*分析处理各种语句*}
    var
      i: integer;

      procedure expression(fsys: symset; var x: item); forward;
      {*分析处理表达式，由参数x返回求值结果的类型*}

      procedure selector(fsys: symset; var v: item);
      var {*处理结构变量：数组下标变量或记录成员变量，由参数v返回求值结果的类型和索引ref*}
        x: item;
        a, j: integer;
      begin {* sy in [lparent, lbrack, period] *}
        repeat
          if sy = period then {*当前符号是点号时*}
          begin
            insymbol; {*读取下一单词符号，处理注释行*} {* field selector *}
            if sy <> ident then
              error(2) {*未读到标识符时报错*}
            else
            begin
              if v.typ <> records then
                error(31) {*参数类型不为记录时报错*}
              else
              begin {*在符号表中寻找当前标识符对应的类型成员变量*}{* search field identifier *}
                j := btab[v.ref].last;
                tab[0].name := id;
                while tab[j].name <> id do
                  j := tab[j].link;
                if j = 0 then
                  error(0); {*未在符号表中找到时报错*}
                v.typ := tab[j].typ;
                v.ref := tab[j].ref; {*更新参数v的信息为该成员变量的信息*}
                a := tab[j].adr; {*获得该成员变量相对类型起始地址的偏移*}
                if a <> 0 then
                  emit1(9, a); {*偏移不为0时产生一条指令将栈顶元素加上偏移*}
              end;
              insymbol; {*读取下一单词符号，处理注释行*}
            end;
          end
          else   {*否则为数组类型*}
          begin {* array selector *}
            if sy <> lbrack then
              error(11);  {*不是'['时报错*}
            repeat  {*循环此过程直到遇到非逗号，处理多维数组*}
              insymbol; {*读取下一单词符号，处理注释行*}
              expression(fsys + [comma, rbrack], x);
              {*调用表达式处理过程处理数组下标，结果保存在x中*}
              if v.typ <> arrays then
                error(28) {*传入的参数类型不为数组时报错*}
              else
              begin
                a := v.ref;  {*获得参数在atab表中的位置*}
                if atab[a].inxtyp <> x.typ then
                  error(26){*下标索引类型与atab表规定的不一致，就报错*}
                else if atab[a].elsize = 1 then
                  emit1(20, a) {*元素长度为1时取下标变量地址的指令*}
                else
                  emit1(21, a); {*元素长度不为1时取下标变量地址的指令*}
                v.typ := atab[a].eltyp; {*更新参数v的信息为该数组元素的信息*}
                v.ref := atab[a].elref;
              end
            until sy <> comma; {*循环此过程直到遇到非逗号，处理多维数组*}
            if sy = rbrack then
              insymbol
            else
            begin
              error(12); {*非']'时报错*}
              if sy = rparent then {*读到右括号时进行容错*}
                insymbol; {*读取下一单词符号，处理注释行*}
            end;
          end
        until not (sy in [lbrack, lparent, period]);
        {*循环直到所有数组下标变量或记录成员变量都处理完成*}
        test(fsys, [], 6); {*测试当前符号是否合法*}
      end {* selector *};

      procedure call(fsys: symset; i: integer);
      var {*处理非标准的过程或函数调用,i为过程或函数名在符号表的索引*}
        x: item;
        lastp, cp, k: integer;
      begin
        emit1(18, i); {*标记栈指令*} {* mark stack *}
        lastp := btab[tab[i].ref].lastpar; {*获得该分程序最后一个参数在符号表中的位置*}
        cp := i; {*获得该分程序名在符号表的位置*}
        if sy = lparent then
        begin {*开始处理实参列表*}{* actual parameter list *}
          repeat
            insymbol; {*读取下一单词符号，处理注释行*}
            if cp >= lastp then
              error(39) {*当前形参符号的位置大于最后一个形参符号的位置时报错*}
            else
            begin
              cp := cp + 1;
              if tab[cp].normal then {*参数为值形参时*}
              begin {* value parameter *}
                expression(fsys + [comma, colon, rparent], x);
                {*分析处理表达式，由参数x返回求值结果的类型*}
                if x.typ = tab[cp].typ then {*实参与形参类型一致*}
                begin
                  if x.ref <> tab[cp].ref then
                    error(36) {*实参与形参ref不一致时报错*}
                  else if x.typ = arrays then {*对于数组或记录生成装入块指令*}
                    emit1(22, atab[x.ref].size)
                  else if x.typ = records then
                    emit1(22, btab[x.ref].vsize);
                end
                else if (x.typ = ints) and (tab[cp].typ = reals) then
                  emit1(26, 0) {*实参为整数，形参为实数，生成转换浮点数指令*}
                else if x.typ <> notyp then
                  error(36); {*实参类型未定义，报错*}
              end
              else {*参数为变量形参时*}
              begin {* variable parameter *}
                if sy <> ident then
                  error(2) {*当前符号不是标识符时报错*}
                else
                begin
                  k := loc(id); {*查找标识符在符号表中的位置*}
                  insymbol; {*读取下一单词符号，处理注释行*}
                  if k <> 0 then {*找到了该标识符时*}
                  begin
                    if tab[k].obj <> vvariable then
                      error(37); {*形参类型不是变量则报错*}
                    x.typ := tab[k].typ;
                    x.ref := tab[k].ref;
                    if tab[k].normal then{*如果该标识符不是变量形参*}
                      emit2(0, tab[k].lev, tab[k].adr) {*生成取地址指令*}
                    else  {*如果该标识符是变量形参*}
                      emit2(1, tab[k].lev, tab[k].adr); {*生成取值指令*}
                    if sy in [lbrack, lparent, period] then
                      selector(fsys + [comma, colon, rparent], x); {*处理结构变量*}
                    if (x.typ <> tab[cp].typ) or (x.ref <> tab[cp].ref) then
                      error(36); {*如果实参与形参的类型或ref不符合，则报错*}
                  end;
                end;
              end; {*variable parameter *}
            end;
            test([comma, rparent], fsys, 6)  {*测试当前符号是否为','')'*}
          until sy <> comma;
          if sy = rparent then
            insymbol
          else
            error(4); {*测试当前不是')'时报错*}
        end;
        if cp < lastp then {*实参数太少报错*}
          error(39); {* too few actual parameters *}
        emit1(19, btab[tab[i].ref].psize - 1); {*生成调用过程或函数的指令*}
        if tab[i].lev < level then
          emit2(3, tab[i].lev, level); {*生成更新display指令*}
      end {* call *};

      function resulttype(a, b: types): types;
      begin {*处理整型或实型两个操作数运算时的类型转换，并返回运行结果的类型*}
        if (a > reals) or (b > reals) then
        begin  {*a或b有一个不为整型或实型或notyp时报错并把返回值设为notyp*}
          error(33);
          resulttype := notyp;
        end
        else if (a = notyp) or (b = notyp) then
          resulttype := notyp
        else if a = ints then
          if b = ints then
            resulttype := ints
          else
          begin {*a是整数，b不是整数，则结果是实数，进行类型转换*}
            resulttype := reals;
            emit1(26, 1);
          end
        else
        begin
          resulttype := reals;
          if b = ints then{*a不是整数，b是整数，则结果是实数，进行类型转换*}
            emit1(26, 0);
        end;
      end {* resulttype *};

      procedure expression(fsys: symset; var x: item);
      var  {*分析处理表达式，由参数x返回求值结果的类型*}
        y: item;
        op: symbol;

        procedure simpleexpression(fsys: symset; var x: item);
        var {*处理简单表达式，由参数x返回求值结果的类型*}
          y: item;
          op: symbol;

          procedure term(fsys: symset; var x: item); {*处理项，由参数x返回结果类型*}
          var
            y: item;
            op: symbol;

            procedure factor(fsys: symset; var x: item); {*处理因子，由参数x返回结果类型*}
            var
              i, f: integer;

              procedure standfct(n: integer); {*处理标准函数调用*}
              var
                ts: typset;
              begin  {* standard function no. n *}
                if sy = lparent then
                  insymbol
                else
                  error(9); {*如果当前符号不是左括号则报错*}
                if n < 17 then {*入口地址小于17时*}
                begin
                  expression(fsys + [rparent], x);
                  {*分析处理表达式，由参数x返回求值结果的类型*}
                  case n of  {*根据不同的函数确定形参的类型，执行不同的操作*}
                    {* abs, sqr *} 0, 2:
                    begin
                      ts := [ints, reals];
                      tab[i].typ := x.typ; {*设置函数返回值的类型*}
                      if x.typ = reals then
                        n := n + 1; {*如果返回值类型是实型，入口地址加1*}
                    end;
                    {* odd, chr *} 4, 5: ts := [ints];
                    {* odr *}         6: ts := [ints, bools, chars];
                    {* succ,pred *} 7, 8:
                    begin
                      ts := [ints, bools, chars];
                      tab[i].typ := x.typ; {*设置函数返回值的类型*}
                    end;
                    {* round,trunc *} 9, 10, 11, 12, 13, 14, 15, 16:
                    {* sin,cos,... *}
                    begin
                      ts := [ints, reals];
                      if x.typ = ints then
                        emit1(26, 0); {*如果参数为整型则需要转化为实型*}
                    end;
                  end; {* case *}
                  if x.typ in ts then
                    emit1(8, n) {*参数类型合法时生成标准函数指令*}
                  else if x.typ <> notyp then
                    error(48);  {*参数类型不为未定义则报错*}
                end
                else
                begin {* n in [17,18] *}
                  if sy <> ident then
                    error(2)  {*参数不是标识符，报错*}
                  else if id <> 'input    ' then
                    error(0)  {*参数名不是’input’，报错*}
                  else
                    insymbol; {*读取下一单词符号，处理注释行*}
                  emit1(8, n); {*生成标准函数指令*}
                end;
                x.typ := tab[i].typ; {*保存函数返回值的类型*}
                if sy = rparent then
                  insymbol
                else
                  error(4); {*如果当前符号不是右括号则报错*}
              end {* standfct *};

            begin {* factor *}
              x.typ := notyp; {*初始化*}
              x.ref := 0;
              test(facbegsys, fsys, 58); {*检查当前符号是否是因子开始符号*}
              while sy in facbegsys do
              begin
                if sy = ident then {*当前符号是标识符时*}
                begin
                  i := loc(id); {*查找标识符在符号表中的位置*}
                  insymbol; {*读取下一单词符号，处理注释行*}
                  with tab[i] do
                    case obj of
                      konstant: {*标识符为常量时*}
                      begin
                        x.typ := typ;
                        x.ref := 0; {*常量ref设为0*}
                        if x.typ = reals then
                          emit1(25, adr){*如果是实数类型，生成装入实数指令*}
                        else
                          emit1(24, adr); {*如果是整型，生成装入字面常量指令*}
                      end;
                      vvariable: {*标识符为变量时*}
                      begin
                        x.typ := typ;
                        x.ref := ref;
                        if sy in [lbrack, lparent, period] then {*标识符为记录或数组时*}
                        begin
                          if normal then
                            f := 0
                          else {*标识符为变量形参时，f设为1*}
                            f := 1;
                          emit2(f, lev, adr); {*生成对应的取地址代码*}
                          selector(fsys, x); {*处理结构变量,由参数x返回求值结果的类型和索引ref*}
                          if x.typ in stantyps then
                            emit(34); {*如果x是标准类型,取以栈顶为地址单元的内容*}
                        end
                        else
                        begin
                          if x.typ in stantyps then {*如果x是标准类型*}
                            if normal then
                              f := 1
                            else
                              f := 2 {*x为变量形参时，f设为2*}
                          else if normal then
                            f := 0 {*x不是标准类型且不是变量形参时，f设为0*}
                          else
                            f := 1; {*x不是标准类型但是变量形参时，f设为1*}
                          emit2(f, lev, adr); {*生成对应的取地址或取值代码*}
                        end;
                      end;
                      typel, prozedure: error(44); {*标识符为类型、过程时报错*}
                      funktion:  {*标识符为函数时*}
                      begin
                        x.typ := typ;  {*设置返回类型*}
                        if lev <> 0 then
                          call(fsys, i){*处理非标准的过程或函数调用，i为标识符在符号表中的位置*}
                        else
                          standfct(adr); {*处理标准函数调用*}
                      end
                    end; {* case,with *}
                end
                else if sy in [charcon, intcon, realcon] then
                begin  {*如果当前符号是字符，整数或者实数常量*}
                  if sy = realcon then
                  begin {*当前符号是实数常量时*}
                    x.typ := reals;
                    enterreal(rnum); {*登录实常数表（rconso表）*}
                    emit1(25, c1); {*将该实数（在实数表中的索引为c1）压入栈顶*}
                  end
                  else  {*当前符号是字符，整数常量时*}
                  begin
                    if sy = charcon then
                      x.typ := chars
                    else
                      x.typ := ints;
                    emit1(24, inum); {*装入字面常量的指令*}
                  end;
                  x.ref := 0; {*常量返回的索引设置为0*}
                  insymbol; {*读取下一单词符号，处理注释行*}
                end
                else if sy = lparent then {*当前符号是‘(’时*}
                begin
                  insymbol; {*读取下一单词符号，处理注释行*}
                  expression(fsys + [rparent], x); {*分析处理表达式，由参数x返回求值结果的类型*}
                  if sy = rparent then
                    insymbol
                  else
                    error(4); {*当前符号不是‘）’时报错*}
                end
                else if sy = notsy then {*当前符号是逻辑非时*}
                begin
                  insymbol; {*读取下一单词符号，处理注释行*}
                  factor(fsys, x); {*递归处理因子，由参数x返回结果类型*}
                  if x.typ = bools then
                    emit(35)  {*x为布尔型生成逻辑非指令*}
                  else if x.typ <> notyp then
                    error(32); {*x不为notyp时报错*}
                end;
                test(fsys, facbegsys, 6); {*测试当前符号是否合法*}
              end; {* while *}
            end {* factor *};

          begin {* term   *}
            factor(fsys + [times, rdiv, idiv, imod, andsy], x);
            {*处理因子，由参数x返回结果类型*}
            while sy in [times, rdiv, idiv, imod, andsy] do
            begin
              op := sy; {*设置运算符*}
              insymbol; {*读取下一单词符号，处理注释行*}
              factor(fsys + [times, rdiv, idiv, imod, andsy], y);
              {*处理因子，由参数y返回结果类型*}
              if op = times then {*运算符是乘号时*}
              begin
                x.typ := resulttype(x.typ, y.typ);
                {*处理整型或实型两个操作数运算时的类型转换，并返回运行结果的类型*}
                case x.typ of {*根据数据类型生成不同指令*}
                  notyp: ;
                  ints: emit(57);
                  reals: emit(60);
                end;
              end
              else if op = rdiv then {*运算符是实数除法时*}
              begin {*先将x，y中的整数转换为实数*}
                if x.typ = ints then
                begin
                  emit1(26, 1);
                  x.typ := reals;
                end;
                if y.typ = ints then
                begin
                  emit1(26, 0);
                  y.typ := reals;
                end;
                if (x.typ = reals) and (y.typ = reals)
                then {*如果x，y都是实型则生成执行实数除法的指令*}
                  emit(61)
                else
                begin
                  if (x.typ <> notyp) and (y.typ <> notyp)
                  then  {*都不为notyp型则报错*}
                    error(33);
                  x.typ := notyp; {*返回值类型设置为notyp*}
                end;
              end
              else if op = andsy then{*运算符是与时*}
              begin
                if (x.typ = bools) and (y.typ = bools)
                then  {*都为布尔型则直接运算*}
                  emit(56)
                else
                begin
                  if (x.typ <> notyp) and (y.typ <> notyp) then
                    error(32); {*都不为notyp型则报错*}
                  x.typ := notyp; {*返回值类型设置为notyp*}
                end;
              end
              else
              begin {*运算符是整除或取余操作时*}{* op in [idiv,imod] *}
                if (x.typ = ints) and (y.typ = ints)
                then   {*都是整型则直接生成对应指令*}
                  if op = idiv then
                    emit(58)
                  else
                    emit(59)
                else
                begin
                  if (x.typ <> notyp) and (y.typ <> notyp) then
                    error(34);  {*都不为notyp型则报错*}
                  x.typ := notyp; {*返回值类型设置为notyp*}
                end;
              end;
            end; {* while *}
          end {* term *};

        begin {* simpleexpression *}
          if sy in [plus, minus] then{*当前符号是加减号时*}
          begin
            op := sy; {*设置运算符*}
            insymbol; {*读取下一单词符号，处理注释行*}
            term(fsys + [plus, minus], x); {*处理项，由参数x返回结果类型*}
            if x.typ > reals then
              error(33)  {*x不为整型或实数或notyp时报错*}
            else if op = minus then
              emit(36);  {*是减号时x取相反数*}
          end
          else
            term(fsys + [plus, minus, orsy], x); {*处理项，由参数x返回结果类型*}
          while sy in [plus, minus, orsy] do {*当前符号是加减或时循环*}
          begin
            op := sy; {*设置运算符*}
            insymbol; {*读取下一单词符号，处理注释行*}
            term(fsys + [plus, minus, orsy], y); {*处理项，由参数y返回结果类型*}
            if op = orsy then {*运算符为或时*}
            begin
              if (x.typ = bools) and (y.typ = bools) then
                emit(51) {*x，y同为bool时就生成or指令*}
              else
              begin
                if (x.typ <> notyp) and (y.typ <> notyp)
                then
                  error(32); {*x,y类型都不是notyp时报错*}
                x.typ := notyp; {*x类型设为notyp*}
              end;
            end
            else {*运算符为加减时*}
            begin
              x.typ := resulttype(x.typ, y.typ); {*处理整型或实型两个操作数运算时的类型转换，并返回结果的类型*}
              case x.typ of {*根据类型和运算符的不同生成不同的加减指令*}
                notyp: ;
                ints: if op = plus then
                    emit(52)
                  else
                    emit(53);
                reals: if op = plus then
                    emit(54)
                  else
                    emit(55)
              end; {* case *}
            end;
          end; {* while *}
        end {* simpleexpression *};

      begin {* expression  *}
        simpleexpression(fsys + [eql, neq, lss, leq, gtr, geq], x);
        {*处理简单表达式，由参数x返回求值结果的类型*}
        if sy in [eql, neq, lss, leq, gtr, geq] then {*当前符号为数值比较类的符号时*}
        begin
          op := sy; {*设置运算符*}
          insymbol; {*读取下一单词符号，处理注释行*}
          simpleexpression(fsys, y); {*处理简单表达式，由参数y返回求值结果的类型*}
          if (x.typ in [notyp, ints, bools, chars]) and (x.typ = y.typ)
          then  {*x，y都为相同的基本数据类型（整型）时*}
            case op of {*根据运算符生成不同的指令*}
              eql: emit(45);
              neq: emit(46);
              lss: emit(47);
              leq: emit(48);
              gtr: emit(49);
              geq: emit(50);
            end
          else
          begin
            if x.typ = ints then
            begin {*x为整数时类型转换为实数*}
              x.typ := reals;
              emit1(26, 1);
            end
            else if y.typ = ints then
            begin {*y为整数时类型转换为实数*}
              y.typ := reals;
              emit1(26, 0);
            end;
            if (x.typ = reals) and (y.typ = reals) then
              case op of {*x，y都为实数时，根据运算符生成不同的比较指令*}
                eql: emit(39);
                neq: emit(40);
                lss: emit(41);
                leq: emit(42);
                gtr: emit(43);
                geq: emit(44);
              end
            else
              error(35);
          end;
          x.typ := bools; {*返回值的类型设置为布尔类型，表示比较结果*}
        end;
      end {* expression *};

      procedure assignment(lv, ad: integer); {*处理赋值语句*}
      var                  {*lv, ad分别为当前变量的level，adr*}
        x, y: item;
        f: integer;
      begin   {* tab[i].obj in [variable,prozedure] *}
        x.typ := tab[i].typ;
        x.ref := tab[i].ref;
        if tab[i].normal then
          f := 0 {*tab[i]不为变量形参时装入变量地址*}
        else
          f := 1; {*tab[i]为变量形参时装入变量值（变量形参的变量值就是实参的地址）*}
        emit2(f, lv, ad); {*生成取值或取数的p代码指令*}
        if sy in [lbrack, lparent, period] then {*当前变量为'[' '(' '.'时*}
          selector([becomes, eql] + fsys, x);
          {*处理结构变量：数组下标变量或记录成员变量，由参数x返回求值结果的类型和索引ref*}
        if sy = becomes then
          insymbol
        else
        begin
          error(51); {*无赋值符号时报错*}
          if sy = eql then {*读到等号进行容错*}
            insymbol; {*读取下一单词符号，处理注释行*}
        end;
        expression(fsys, y); {*分析处理表达式，由参数y返回求值结果的类型*}
        if x.typ = y.typ then {*两个操作数类型一致*}
          if x.typ in stantyps then {*都为标准数据类型时*}
            emit(38) {*生成赋值指令STO*}
          else if x.ref <> y.ref then
            error(46) {*不为标准类型且索引不同则报错*}
          else if x.typ = arrays then
            emit1(23, atab[x.ref].size) {*类型是数组时生成复制数组表中项的指令CPB*}
          else
            emit1(23, btab[x.ref].vsize) {*类型是记录时生成复制btab表中项的指令CPB*}
        else if (x.typ = reals) and (y.typ = ints) then
        begin  {*整数向浮点数赋值时*}
          emit1(26, 0); {*生成转换浮点数指令*}
          emit(38); {*生成赋值指令STO*}
        end
        else if (x.typ <> notyp) and (y.typ <> notyp) then
          error(46); {*如果两个数的类型都不是未定义则报错*}
      end {* assignment *};

      procedure compoundstatement; {*处理复合语句*}
      begin
        insymbol; {*读取下一单词符号，处理注释行*}
        statement([semicolon, endsy] + fsys); {*间接递归分析处理语句*}
        while sy in [semicolon] + statbegsys do {*符号是语句开始符号或分号时*}
        begin
          if sy = semicolon then
            insymbol
          else
            error(14); {*语句结尾无分号报错*}
          statement([semicolon, endsy] + fsys); {*间接递归分析处理语句*}
        end;
        if sy = endsy then
          insymbol
        else
          error(57); {*复合语句不以end结尾则报错*}
      end {* compoundstatement *};

      procedure ifstatement; {*处理if语句*}
      var
        x: item;
        lc1, lc2: integer;
      begin
        insymbol; {*读取下一单词符号，处理注释行*}
        expression(fsys + [thensy, dosy], x);
        {*分析处理表达式，由参数x返回求值结果的类型*}
        if not (x.typ in [bools, notyp]) then
          error(17); {*表达式结果不合规范报错*}
        lc1 := lc; {*获得当前P代码表索引*}  {*获得当前P代码表索引*}
        emit(11);  {* 生成jpc指令 *}
        if sy = thensy then
          insymbol
        else
        begin   {*当前符号不是then时先报错*}
          error(52);
          if sy = dosy then {*当前符号是do时容错*}
            insymbol; {*读取下一单词符号，处理注释行*}
        end;
        statement(fsys + [elsesy]); {*分析处理各种语句*}
        if sy = elsesy then
        begin {*存在else部分*}
          insymbol; {*读取下一单词符号，处理注释行*}
          lc2 := lc; {*获得当前P代码表索引*}
          emit(10); {*生成无条件转移指令*}
          code[lc1].y := lc; {*设置jpc跳转指令的跳转位置*}
          statement(fsys); {*分析处理各种语句*}
          code[lc2].y := lc; {*设置无条件跳转指令的跳转位置*}
        end
        else
          code[lc1].y := lc; {*设置jpc跳转指令的跳转位置*}
      end {* ifstatement *};

      procedure casestatement; {*处理case语句*}
      var
        x: item;
        i, j, k, lc1: integer; {*i为标号个数，j为分支个数*}
        casetab: array[1..csmax] of packed record
          val, lc: index {*记录每个标号的值和对应代码地址*}
        end;
        exittab: array[1..csmax] of integer;
         {*记录每个分支结束时对应代码地址*}

        procedure caselabel; {*处理case语句中的标号，将各标号对应的目标代码人口地址填人casetab表中，并检查标号有无重复定义*}
        var
          lab: conrec;
          k: integer;
        begin
          constant(fsys + [comma, colon], lab);
          {*处理程序中出现的常量，并由参数lab返回该常量的类型和数值*}
          if lab.tp <> x.typ then
            error(47) {*类型不一致报错*}
          else if i = csmax then
            fatal(6){*casetab表溢出报错*}
          else
          begin
            i := i + 1;
            k := 0;
            casetab[i].val := lab.i;
            casetab[i].lc := lc;
            repeat
              k := k + 1
            until casetab[k].val = lab.i; {*在表中查找该标号*}
            if k < i then {*标号重复定义，报错*}
              error(1); {* multiple definition *}
          end;
        end {* caselabel *};

        procedure onecase; {*处理case语句中的一个分支*}
        begin
          if sy in constbegsys then {*常量开始符号*}
          begin
            caselabel; {*处理case语句中的标号，将各标号对应的目标代码人口地址填人casetab表中，并检查标号有无重复定义*}
            while sy = comma do {*处理同一分支的多个标号*}
            begin
              insymbol; {*读取下一单词符号，处理注释行*}
              caselabel; {*处理case语句中的标号，将各标号对应的目标代码人口地址填人casetab表中，并检查标号有无重复定义*}
            end;
            if sy = colon then
              insymbol
            else
              error(5); {*无冒号则报错*}
            statement([semicolon, endsy] + fsys); {*分析处理各种语句*}
            j := j + 1; {*j为分支个数*}
            exittab[j] := lc; {*将当前代码位置记录到exittab表中*}
            emit(10); {*生成无条件转移指令*}
          end;
        end {* onecase *};

      begin  {* casestatement  *}
        insymbol; {*读取下一单词符号，处理注释行*}
        i := 0;
        j := 0;
        expression(fsys + [ofsy, comma, colon], x); {*处理表达式，用x存该表达式的返回值*}
        if not (x.typ in [ints, bools, chars, notyp]) then
          error(23); {*返回值的类型不是整型、布尔型、字符型或者notyp时报错*}
        lc1 := lc; {*获得当前P代码表索引*}
        emit(12); {*生成跳转代码，转移到y，查找情况表*}{*jmpx*}
        if sy = ofsy then
          insymbol
        else
          error(8);
        onecase; {*处理case语句中的一个分支*}
        while sy = semicolon do {*分号为每个分支的结尾*}
        begin {*依次处理每个分支*}
          insymbol; {*读取下一单词符号，处理注释行*}
          onecase; {*处理case语句中的一个分支*}
        end;
        code[lc1].y := lc; {*回填跳转指令的地址为当前情况表的地址*}
        for k := 1 to i do
        begin {*依次处理所有的标号，生成情况表登记项*}
          emit1(13, casetab[k].val);
          emit1(13, casetab[k].lc);
        end;
        emit1(10, 0); {*无条件转移指令，x不满足所有标号时，跳到0*}
        for k := 1 to j do
          code[exittab[k]].y := lc; {*回填每个分支结尾的跳转指令为当前地址*}
        if sy = endsy then
          insymbol
        else
          error(57); {*无end报错*}
      end {* casestatement *};

      procedure repeatstatement; {*处理repeat语句*}
      var
        x: item;
        lc1: integer;
      begin
        lc1 := lc; {*获得当前P代码表索引*}
        insymbol; {*读取下一单词符号，处理注释行*}
        statement([semicolon, untilsy] + fsys); {*分析处理各种语句*}
        while sy in [semicolon] + statbegsys do
        begin
          if sy = semicolon then
            insymbol
          else
            error(14);
          statement([semicolon, untilsy] + fsys); {*分析处理各种语句*}
        end;
        if sy = untilsy then
        begin
          insymbol; {*读取下一单词符号，处理注释行*}
          expression(fsys, x); {*分析处理表达式，由参数x返回求值结果的类型*}
          if not (x.typ in [bools, notyp]) then
            error(17); {*表达式结果类型检查*}
          emit1(11, lc1); {*栈顶内容为假，转移到lc1*}
        end
        else
          error(53); {*无until关键字时报错*}
      end {* repeatstatement *};

      procedure whilestatement; {*处理while语句*}
      var
        x: item;
        lc1, lc2: integer;
      begin
        insymbol; {*读取下一单词符号，处理注释行*}
        lc1 := lc; {*获得当前P代码表索引*}
        expression(fsys + [dosy], x); {*分析处理表达式，由参数x返回求值结果的类型*}
        if not (x.typ in [bools, notyp]) then
          error(17); {*表达式结果类型检查*}
        lc2 := lc; {*获得当前P代码表索引*}
        emit(11); {*栈顶内容为假，跳转*}
        if sy = dosy then
          insymbol
        else
          error(54); {*无do关键字报错*}
        statement(fsys); {*分析处理各种语句*}
        emit1(10, lc1); {*无条件转移到lc1语句开头*}
        code[lc2].y := lc; {*回填lc2处跳转地址为lc*}
      end {* whilestatement *};

      procedure forstatement; {*处理for语句*}
      var
        cvt: types; {*循环计数变量*}
        x: item;
        i, f, lc1, lc2: integer;
      begin
        insymbol; {*读取下一单词符号，处理注释行*}
        if sy = ident then {*当前符号为标识符*}
        begin
          i := loc(id); {*查找标识符在符号表中的位置*}
          insymbol; {*读取下一单词符号，处理注释行*}
          if i = 0 then
            cvt := ints {*未找到标识符，循环计数变量设为整型*}
          else if tab[i].obj = vvariable then
          begin
            cvt := tab[i].typ; {*设置计数变量类型*}
            if not tab[i].normal then
              error(37) {*计数变量为变量形参时，报错*}
            else
              emit2(0, tab[i].lev, tab[i].adr); {*生成取地址指令*}
            if not (cvt in [notyp, ints, bools, chars]) then
              error(18); {*计数变量类型检查*}
          end
          else
          begin {*计数变量不为变量，报错并将其视为整型*}
            error(37);
            cvt := ints;
          end;
        end
        else{*当前符号不为标识符*}
          skip([becomes, tosy, downtosy, dosy] + fsys, 2);
          {*跳读源程序，直至取来的符号属于给出的符号集为止，并打印出错标志*}
        if sy = becomes then
        begin
          insymbol; {*读取下一单词符号，处理注释行*}
          expression([tosy, downtosy, dosy] + fsys, x);
          {*分析处理表达式，由参数x返回求值结果的类型*}
          if x.typ <> cvt then
            error(19); {*类型一致检查*}
        end
        else {*当前符号不为赋值符*}
          skip([tosy, downtosy, dosy] + fsys, 51);
          {*跳读源程序，直至取来的符号属于给出的符号集为止，并打印出错标志*}
        f := 14; {*设置生成指令的操作码为14，增量步长型*}
        if sy in [tosy, downtosy] then
        begin
          if sy = downtosy then
            f := 16; {*如果是downto设置生成指令的操作码为16，减量步长型*}
          insymbol; {*读取下一单词符号，处理注释行*}
          expression([dosy] + fsys, x);
          {*分析处理表达式，由参数x返回求值结果的类型*}
          if x.typ <> cvt then
            error(19); {*类型一致检查*}
        end
        else
          skip([dosy] + fsys, 55);
          {*跳读源程序，直至取来的符号属于给出的符号集为止，并打印出错标志*}
        lc1 := lc; {*获得当前P代码表索引*}
        emit(f); {*生成指令，for循环体的入口测试指令*}
        if sy = dosy then
          insymbol
        else
          error(54);
        lc2 := lc; {*获得当前P代码表索引*}
        statement(fsys); {*分析处理各种语句*}
        emit1(f + 1, lc2); {*生成对应循环体的再入测试指令，跳转地址为lc2*}
        code[lc1].y := lc; {*回填lc1处跳转地址为当前地址*}
      end {* forstatement *};

      procedure standproc(n: integer); {*处理标准输入输出过程调用*}
      var
        i, f: integer;
        x, y: item;
      begin
        case n of
          1, 2:
          begin {* read *} {*n为1,2表示读入*}
            if not iflag then
            begin
              error(20); {*标准输入flag为false报错*}
              iflag := true; {*重置标准输入flag*}
            end;
            if sy = lparent then
            begin
              repeat {*依次处理每个参数*}
                insymbol; {*读取下一单词符号，处理注释行*}
                if sy <> ident then
                  error(2) {*参数非标识符报错*}
                else
                begin  {*参数为标识符时*}
                  i := loc(id); {*查找标识符在符号表中的位置*}
                  insymbol; {*读取下一单词符号，处理注释行*}
                  if i <> 0 then {*查到该标识符*}
                    if tab[i].obj <> vvariable then
                      error(37) {*标识符不是变量则报错*}
                    else
                    begin
                      x.typ := tab[i].typ;
                      x.ref := tab[i].ref;
                      if tab[i].normal then
                        f := 0
                      else
                        f := 1; {*变量形参*}
                      emit2(f, tab[i].lev, tab[i].adr); {*生成取地址指令*}
                      if sy in [lbrack, lparent, period] then
                        selector(fsys + [comma, rparent], x); {*处理结构变量*}
                      if x.typ in [ints, reals, chars, notyp] then
                        emit1(27, ord(x.typ)) {*对指定类型生成读指令*}
                      else
                        error(41); {*类型不合要求则报错*}
                    end;
                end;
                test([comma, rparent], fsys, 6);
              until sy <> comma;
              if sy = rparent then
                insymbol
              else
                error(4); {*无右括号报错*}
            end;
            if n = 2 then
              emit(62); {*n为2时生成读完一行的指令*}
          end;
          3, 4: {*n为3,4表示写操作*}
          begin {* write *}
            if sy = lparent then
            begin
              repeat {*依次处理每个参数*}
                insymbol; {*读取下一单词符号，处理注释行*}
                if sy = stringcon then
                begin {*参数为字符串时*}
                  emit1(24, sleng); {*装入字符串长度*}
                  emit1(28, inum); {*写字符*}
                  insymbol; {*读取下一单词符号，处理注释行*}
                end
                else
                begin
                  expression(fsys + [comma, colon, rparent], x);
                  {*分析处理表达式，由参数x返回求值结果的类型*}
                  if not (x.typ in stantyps) then
                    error(41); {*类型检查*}
                  if sy = colon then {*读到冒号，后接域宽*}
                  begin
                    insymbol; {*读取下一单词符号，处理注释行*}
                    expression(fsys + [comma, colon, rparent], y);
                    {*分析处理表达式，由参数y返回求值结果的类型*}
                    if y.typ <> ints then
                      error(43); {*域宽不为整数报错*}
                    if sy = colon then  {*双域宽，必须为实数*}
                    begin
                      if x.typ <> reals then
                        error(42); {*类型检查*}
                      insymbol; {*读取下一单词符号，处理注释行*}
                      expression(fsys + [comma, rparent], y);
                      {*分析处理表达式，由参数y返回求值结果的类型*}
                      if y.typ <> ints then
                        error(43); {*字宽不为整数报错*}
                      emit(37); {*生成写实数指令，给定域宽*}
                    end
                    else
                      emit1(30, ord(x.typ)); {*生成写指令，给定域宽*}
                  end
                  else
                    emit1(29, ord(x.typ)); {*生成写指令，隐含域宽*}
                end
              until sy <> comma;
              if sy = rparent then
                insymbol
              else
                error(4);
            end;
            if n = 4 then
              emit(63); {*n为4，生成换行写指令*}
          end; {* write *}
        end {* case *};
      end {* standproc *};

    begin {* statement *}
      if sy in statbegsys + [ident] then {*当前符号为语句初始符号或标识符时*}
        case sy of
          ident: {*当前符号为标识符时*}
          begin
            i := loc(id); {*查找标识符在符号表中的位置*}
            insymbol; {*读取下一单词符号，处理注释行*}
            if i <> 0 then {*在符号表中找到该标识符*}
              case tab[i].obj of {*根据标识符种类进行不同处理*}
                konstant, typel: error(45); {*常量和类型直接报错*}
                vvariable: assignment(tab[i].lev, tab[i].adr); {*变量处理赋值语句*}
                prozedure: if tab[i].lev <> 0 then
                    call(fsys, i) {*处理非标准的过程调用*}
                  else
                    standproc(tab[i].adr); {*处理标准输入输出过程调用*}
                funktion: if tab[i].ref = display[level] then {*函数在btab表的指针和display表中保存的指针一致时*}
                    assignment(tab[i].lev + 1, 0) {*执行函数赋值语句*}
                  else
                    error(45)
              end; {* case *}
          end;
          beginsy: compoundstatement; {*当前符号为begin,处理复合语句*}
          ifsy: ifstatement; {*处理if语句*}
          casesy: casestatement; {*处理case语句*}
          whilesy: whilestatement; {*处理while语句*}
          repeatsy: repeatstatement; {*处理repeat语句*}
          forsy: forstatement; {*处理for语句*}
        end;  {* case *}
      test(fsys, [], 14); {*测试当前符号是否合法*}
    end {* statement *};

  begin  {* block *}
    dx := 5; {*每个分程序在运行栈数据区开头留出5个单元*}
    prt := t; {*获得当前符号表的索引，即分程序名对应的位置*}
    if level > lmax then
      fatal(5); {*display表溢出时报错*}
    test([lparent, colon, semicolon], fsys, 14); {*测试当前符号是否是左括号、冒号和分号*}
    enterblock; {*登录分程序表btab*}
    prb := b; {*获得当前btab表的索引*}
    display[level] := b; {*登录分程序索引表*}
    tab[prt].typ := notyp;
    tab[prt].ref := prb;
    if (sy = lparent) and (level > 1) then {*当前读到的符号为左括号且分程序层次大于时*}
      parameterlist; {*处理过程或函数说明中的形参表，将形参及其有关信息登录到符号表中*}
    btab[prb].lastpar := t; {*指向最后一个参数在tab表的位置*}
    btab[prb].psize := dx; {*内务信息区、参数的总大小*}
    if isfun then {*该分程序为函数时*}
      if sy = colon then {*当前符号为冒号时*}
      begin
        insymbol; {*读取下一单词符号，处理注释行*} {* function type *}
        if sy = ident then {*当前读到的符号为标识符*}
        begin
          x := loc(id); {*查找标识符在符号表中的位置*}
          insymbol; {*读取下一单词符号，处理注释行*}
          if x <> 0 then {*在符号表中找到了该标识符*}
            if tab[x].typ in stantyps then {*函数返回值类型为标准类型时*}
              tab[prt].typ := tab[x].typ {*设置函数的返回值类型*}
            else
              error(15);
        end
        else {*冒号后未读到标识符时，跳过无用符直至取来的符号属于给出的符号集为止*}
          skip([semicolon] + fsys, 2); {*指定符号集多了分号*}
      end
      else
        error(5); {*无冒号时报错*}
    if sy = semicolon then
      insymbol {*读到分号时读下一符号*}
    else
      error(14); {*否则报错*}

    repeat {*重复此过程直到读到表达式初始符号*}
      if sy = constsy then {*当前符号为常量保留字const时*}
        constdec; {*处理常量定义，将常量名及其相应信息填人符号表*}
      if sy = typesy then {*当前符号为类型保留字type时*}
        typedeclaration; {*处理类型定义，并将类型名及其相应信息填人符号表*}
      if sy = varsy then {*当前符号为变量保留字var时*}
        variabledeclaration; {*处理变量定义，并将变量名及其相应信息填人符号表*}
      btab[prb].vsize := dx; {*内务信息区、参数、局部变量的总大小*}
      while sy in [procsy, funcsy] do
        procdeclaration; {*处理过程或函数说明，将过程、函数名填入符号表，递归调用block分析处理下一层分程序*}
      test([beginsy], blockbegsys + statbegsys, 56)
    until sy in statbegsys; {*重复此过程直到读到表达式初始符号*}

    tab[prt].adr := lc; {*当前分程序的adr填入目标代码的入口地址*}
    insymbol; {*读取下一单词符号，处理注释行*}
    statement([semicolon, endsy] + fsys); {*分析处理各种语句*}
    while sy in [semicolon] + statbegsys do {*当前符号为分号或语句初始符号时*}
    begin
      if sy = semicolon then
        insymbol
      else
        error(14); {*语句结尾无分号时报错*}
      statement([semicolon, endsy] + fsys); {*分析处理各种语句*}
    end;
    if sy = endsy then
      insymbol
    else
      error(57); {*分程序结尾无end时报错*}
    test(fsys + [period], [], 6); {*测试当前符号是否是fsys符号集的符号或句号*}
  end {* block *};



  procedure interpret; {*P代码解释执行程序*}
  var
    ir: order;         {*指令缓冲区*}{* instruction buffer *}
    pc: integer;       {*指令寄存器*} {* program counter *}
    t: integer;        {*栈顶指针*}{* top stack index *}
    b: integer;        {*基地址指针*}{* base index *}
    h1, h2, h3: integer;
    lncnt, ocnt, blkcnt, chrcnt: integer; {*各个计数器*}     {* counters *}
    ps: (run, fin, caschk, divchk, inxchk, stkchk, linchk, lngchk, redchk); {*指令的执行状态ps*}
    fld: array [1..4] of integer; {*默认域宽*} {* default field widths *}
    display: array[0..lmax] of integer; {*分程序索引表，索引为level，表项内容为指向btab表的指针 *}
    s: array[1..stacksize] of   {*运行栈S数据结构*}{* blockmark:     *}
      record
      case cn: types of        {* s[b+0] = fct result *}
        ints: (i: integer);    {* s[b+1] = return adr *}
        reals: (r: real);      {* s[b+2] = static link *}
        bools: (b: boolean);   {* s[b+3] = dynamic link *}
        chars: (c: char)       {* s[b+4] = table index *}
    end;

    procedure dump; {*程序运行时，卸出打印现场剖析信息（display，t、b及运行栈S的内容）*}
    var
      p, h3: integer;
    begin
      h3 := tab[h2].lev; {*获取当前所在层次*}
      writeln(psout); {*在编译输出文件中输出空行*}
      writeln(psout); {*在编译输出文件中输出空行*}
      writeln(psout, '       calling ', tab[h2].name);
      writeln(psout, '         level ', h3: 4);
      writeln(psout, ' start of code ', pc: 4);
      writeln(psout); {*在编译输出文件中输出空行*}
      writeln(psout); {*在编译输出文件中输出空行*}
      writeln(psout, ' contents of display ');
      writeln(psout); {*在编译输出文件中输出空行*}
      for p := h3 downto 0 do {*向外输出display表每一层的信息*}
        writeln(psout, p: 4, display[p]: 6);
      writeln(psout); {*在编译输出文件中输出空行*}
      writeln(psout); {*在编译输出文件中输出空行*}
      writeln(psout, ' top of stack  ', t: 4, ' frame base ': 14, b: 4); {*输出栈顶指针和基地址指针*}
      writeln(psout); {*在编译输出文件中输出空行*}
      writeln(psout); {*在编译输出文件中输出空行*}
      writeln(psout, ' stack contents ': 20);
      writeln(psout); {*在编译输出文件中输出空行*}
      for p := t downto 1 do {*依次输出运行栈S中信息*}
        writeln(psout, p: 14, s[p].i: 8);
      writeln(psout, '< = = = >': 22);
    end; {*dump *}

    procedure inter0; {*指令操作码为0-9时*}
    begin
      case ir.f of
        0:
        begin {* load addrss *}
          t := t + 1;
          if t > stacksize then
            ps := stkchk{*标记栈溢出错误*}
          else
            s[t].i := display[ir.x] + ir.y; {*绝对地址=起始地址+相对位移*}
        end;
        1:
        begin  {* load value *}
          t := t + 1;
          if t > stacksize then
            ps := stkchk{*标记栈溢出错误*}
          else
            s[t] := s[display[ir.x] + ir.y];
        end;
        2:
        begin  {* load indirect *}
          t := t + 1;
          if t > stacksize then
            ps := stkchk{*标记栈溢出错误*}
          else
            s[t] := s[s[display[ir.x] + ir.y].i]; {*先取值，然后以该值为地址，再次取值*}
        end;
        3:
        begin  {* update display *}
          h1 := ir.y; {*调用过程的层次*}
          h2 := ir.x; {*被调用过程的层次*}
          h3 := b;
          repeat  {*根据S栈中各数据区的静态链重填display表*}
            display[h1] := h3;
            h1 := h1 - 1;
            h3 := s[h3 + 2].i
          until h1 = h2;
        end;
        8: case ir.y of  {*执行标准函数0-18*}
            0: s[t].i := abs(s[t].i);
            1: s[t].r := abs(s[t].r);
            2: s[t].i := sqr(s[t].i);
            3: s[t].r := sqr(s[t].r);
            4: s[t].b := odd(s[t].i); {*整数判断奇偶*}
            5: s[t].c := chr(s[t].i); {*ASCII码转字符*}
            6: s[t].i := ord(s[t].c); {*字符转ASCII码*}
            7: s[t].c := succ(s[t].c); {*求字符的后继*}
            8: s[t].c := pred(s[t].c); {*求字符的前驱*}
            9: s[t].i := round(s[t].r); {*四舍五入*}
            10: s[t].i := trunc(s[t].r); {*取整*}
            11: s[t].r := sin(s[t].r);
            12: s[t].r := cos(s[t].r);
            13: s[t].r := exp(s[t].r);
            14: s[t].r := ln(s[t].r);
            15: s[t].r := sqrt(s[t].r);
            16: s[t].r := arcTan(s[t].r);
            17:
            begin
              t := t + 1;
              if t > stacksize then
                ps := stkchk{*标记栈溢出错误*}
              else
                s[t].b := eof(prd); {*判断是否读到文件尾*}
            end;
            18:
            begin
              t := t + 1;
              if t > stacksize then
                ps := stkchk{*标记栈溢出错误*}
              else
                s[t].b := eoln(prd); {*判断是否读完该行*}
            end;
          end;
        9: s[t].i := s[t].i + ir.y; {*栈顶元素增加偏移量y*}
      end; {* case ir.y *}
    end; {* inter0 *}

    procedure inter1; {*指令操作码为10-19*}
    var
      h3, h4: integer;
    begin
      case ir.f of
        10: pc := ir.y; {* jump *}
        11:
        begin  {* conditional jump *}
          if not s[t].b then
            pc := ir.y; {*栈顶内容s[t].b为假时跳转*}
          t := t - 1;
        end;
        12:
        begin {*转移到y，查找情况表*}{* switch *}
          h1 := s[t].i;
          t := t - 1;
          h2 := ir.y; {*跳转到的情况表地址*}
          h3 := 0;
          repeat
            if code[h2].f <> 13 then
            begin
              h3 := 1;
              ps := caschk; {*设置ps为case执行出错*}
            end
            else if code[h2].y = h1 then {*case值匹配，跳转到下一指令执行*}
            begin
              h3 := 1;
              pc := code[h2 + 1].y;
            end
            else
              h2 := h2 + 2 {*跳到情况表下一项*}
          until h3 <> 0;
        end;
        14:
        begin {*步增型for循环入口测试*}{* for1up *}
          h1 := s[t - 1].i; {*h1为初始值*}
          if h1 <= s[t].i then {*执行循环*}
            s[s[t - 2].i].i := h1 {*保存初始值*}
          else
          begin  {*跳转到for循环结束的位置*}
            t := t - 3;
            pc := ir.y;
          end;
        end;
        15:
        begin {*步增型for循环再入测试*}{* for2up *}
          h2 := s[t - 2].i;
          h1 := s[h2].i + 1;
          if h1 <= s[t].i then
          begin {*循环*}
            s[h2].i := h1;
            pc := ir.y; {*跳转到for开始的位置*}
          end
          else {*不循环*}
            t := t - 3;
        end;
        16:
        begin  {*步减型for循环入口测试*}{* for1down *}
          h1 := s[t - 1].i;
          if h1 >= s[t].i then
            s[s[t - 2].i].i := h1
          else
          begin
            pc := ir.y;
            t := t - 3;
          end;
        end;
        17:
        begin  {*步减型for循环再入测试*}{* for2down *}
          h2 := s[t - 2].i;
          h1 := s[h2].i - 1;
          if h1 >= s[t].i then
          begin
            s[h2].i := h1;
            pc := ir.y;
          end
          else
            t := t - 3;
        end;
        18:
        begin{*标记栈*}{* mark stack *}
          h1 := btab[tab[ir.y].ref].vsize; {*获得数据区长度*}
          if t + h1 > stacksize then
            ps := stkchk{*标记栈溢出错误*}
          else
          begin
            t := t + 5; {*留内务信息区*}
            s[t - 1].i := h1 - 1;
            s[t].i := ir.y; {*记录被调过程在tab的位置*}
          end;
        end;
        19:
        begin{*调用用户过程或函数*}{* call *}
          h1 := t - ir.y;  {*h1为当前分程序数据区基地址*}{* h1 points to base *}
          h2 := s[h1 + 4].i;  {*h2为被调过程在tab的位置*}{* h2 points to tab *}
          h3 := tab[h2].lev;
          display[h3 + 1] := h1;
          h4 := s[h1 + 3].i + h1; {*计算栈顶指针t*}
          s[h1 + 1].i := pc; {*返回地址ra*}
          s[h1 + 2].i := display[h3]; {*静态链接层次*}
          s[h1 + 3].i := b; {*动态链接层次*}
          for h3 := t + 1 to h4 do
            s[h3].i := 0;  {*初始化局部变量*}
          b := h1;
          t := h4;
          pc := tab[h2].adr;
          if stackdump then
            dump; {*程序运行时，卸出打印现场剖析信息（display，t、b及运行栈S的内容）*}
        end;
      end; {* case *}
    end; {* inter1 *}

    procedure inter2; {*指令操作码为20-29*}
    begin
      case ir.f of
        20:
        begin{*取下标变量地址，元素长度为1*}{* index1 *}
          h1 := ir.y;  {*h1指向数组在当前atab的基地址*}{* h1 points to atab *}
          h2 := atab[h1].low;
          h3 := s[t].i;
          if h3 < h2 then
            ps := inxchk {*标记数组越界错误*}
          else if h3 > atab[h1].high then
            ps := inxchk {*标记数组越界错误*}
          else
          begin
            t := t - 1;
            s[t].i := s[t].i + (h3 - h2);
          end;
        end;
        21:
        begin{*取下标变量地址*}{* index *}
          h1 := ir.y; {*h1指向数组在当前atab的基地址*}{* h1 points to atab *}
          h2 := atab[h1].low;
          h3 := s[t].i;
          if h3 < h2 then
            ps := inxchk  {*标记数组越界错误*}
          else if h3 > atab[h1].high then
            ps := inxchk  {*标记数组越界错误*}
          else
          begin
            t := t - 1;
            s[t].i := s[t].i + (h3 - h2) * atab[h1].elsize;
          end;
        end;
        22:
        begin {*装入块*}{* load block *}
          h1 := s[t].i;
          t := t - 1;
          h2 := ir.y + t; {*获得需要分配的空间的最大位置*}
          if h2 > stacksize then
            ps := stkchk{*标记栈溢出错误*}
          else
            while t < h2 do
            begin
              t := t + 1;
              s[t] := s[h1];
              h1 := h1 + 1;
            end;
        end;
        23:
        begin{*复制块*}{* copy block *}
          h1 := s[t - 1].i;
          h2 := s[t].i;
          h3 := h1 + ir.y; {*获得需要赋值的空间的最大位置*}
          while h1 < h3 do
          begin
            s[h1] := s[h2];
            h1 := h1 + 1;
            h2 := h2 + 1;
          end;
          t := t - 2;
        end;
        24:
        begin{*装入字面常量*}{* literal *}
          t := t + 1;
          if t > stacksize then
            ps := stkchk{*标记栈溢出错误*}
          else
            s[t].i := ir.y;
        end;
        25:
        begin{*装入实数*}{* load real *}
          t := t + 1;
          if t > stacksize then
            ps := stkchk{*标记栈溢出错误*}
          else
            s[t].r := rconst[ir.y];
        end;
        26:
        begin{*转换浮点数*}{* float *}
          h1 := t - ir.y;
          s[h1].r := s[h1].i;
        end;
        27:
        begin{*读操作,y为类型*}{* read *}
          if eof(prd) then
            ps := redchk {*标记读到文件尾*}
          else
            case ir.y of
              1: read(prd, s[s[t].i].i);
              2: read(prd, s[s[t].i].r);
              4: read(prd, s[s[t].i].c);
            end;
          t := t - 1;
        end;
        28:
        begin{*写字符串*}{* write string *}
          h1 := s[t].i;
          h2 := ir.y;
          t := t - 1;
          chrcnt := chrcnt + h1;
          if chrcnt > lineleng then
            ps := lngchk; {*标记输出行长度超范围的错误*}
          repeat
            write(prr, stab[h2]);
            h1 := h1 - 1;
            h2 := h2 + 1
          until h1 = 0;
        end;
        29:
        begin{*写-隐含域宽*}{* write1 *}
          chrcnt := chrcnt + fld[ir.y];
          if chrcnt > lineleng then
            ps := lngchk {*标记输出行长度超范围的错误*}
          else
            case ir.y of
              1: write(prr, s[t].i: fld[1]);
              2: write(prr, s[t].r: fld[2]);
              3: if s[t].b then
                  write('true')
                else
                  write('false');
              4: write(prr, chr(s[t].i));
            end;
          t := t - 1;
        end;
      end; {* case *}
    end; {* inter2 *}

    procedure inter3; {*指令操作码为30-39*}
    begin
      case ir.f of
        30:
        begin{*写-给定域宽*}{* write2 *}
          chrcnt := chrcnt + s[t].i;
          if chrcnt > lineleng then
            ps := lngchk {*标记输出行长度超范围的错误*}
          else
            case ir.y of
              1: write(prr, s[t - 1].i: s[t].i);
              2: write(prr, s[t - 1].r: s[t].i);
              3: if s[t - 1].b then
                  write('true')
                else
                  write('false');
            end;
          t := t - 2;
        end;
        31:ps := fin; {*停止*}
        32:
        begin{*退出过程*}{* exit procedure *}
          t := b - 1;
          pc := s[b + 1].i; {*返回地址*}
          b := s[b + 3].i; {*动态链接*}
        end;
        33:
        begin{*退出函数*}{* exit function *}
          t := b;
          pc := s[b + 1].i;
          b := s[b + 3].i;
        end;
        34: s[t] := s[s[t].i]; {*取栈顶单元内容为地址的单元内容*}
        35: s[t].b := not s[t].b; {*逻辑非*}
        36: s[t].i := -s[t].i; {*求相反数*}
        37:
        begin {*写实数-给定域宽*}
          chrcnt := chrcnt + s[t - 1].i;
          if chrcnt > lineleng then
            ps := lngchk {*标记输出行长度超范围*}
          else
            write(prr, s[t - 2].r: s[t - 1].i: s[t].i);
          t := t - 3;
        end;
        38:
        begin{*栈顶内容存入一次栈顶单元为地址的单元*}{* store *}
          s[s[t - 1].i] := s[t];
          t := t - 2;
        end;
        39:
        begin{*实数等于比较*}
          t := t - 1;
          s[t].b := s[t].r = s[t + 1].r;
        end;
      end; {* case *}
    end; {* inter3 *}

    procedure inter4; {*指令操作码为40-49*}
    begin
      case ir.f of
        40:
        begin{*实数不等比较*}
          t := t - 1;
          s[t].b := s[t].r <> s[t + 1].r;
        end;
        41:
        begin{*实数小于比较*}
          t := t - 1;
          s[t].b := s[t].r < s[t + 1].r;
        end;
        42:
        begin{*实数小于等于比较*}
          t := t - 1;
          s[t].b := s[t].r <= s[t + 1].r;
        end;
        43:
        begin{*实数大于比较*}
          t := t - 1;
          s[t].b := s[t].r > s[t + 1].r;
        end;
        44:
        begin{*实数大于等于比较*}
          t := t - 1;
          s[t].b := s[t].r >= s[t + 1].r;
        end;
        45:
        begin{*整型相等比较*}
          t := t - 1;
          s[t].b := s[t].i = s[t + 1].i;
        end;
        46:
        begin{*整型不等比较*}
          t := t - 1;
          s[t].b := s[t].i <> s[t + 1].i;
        end;
        47:
        begin{*整型小于比较*}
          t := t - 1;
          s[t].b := s[t].i < s[t + 1].i;
        end;
        48:
        begin{*整型小于等于比较*}
          t := t - 1;
          s[t].b := s[t].i <= s[t + 1].i;
        end;
        49:
        begin{*整型大于比较*}
          t := t - 1;
          s[t].b := s[t].i > s[t + 1].i;
        end;
      end; {* case *}
    end; {* inter4 *}

    procedure inter5; {*指令操作码为50-59*}
    begin
      case ir.f of
        50:
        begin{*整型大于等于比较*}
          t := t - 1;
          s[t].b := s[t].i >= s[t + 1].i;
        end;
        51:
        begin{*逻辑或*}
          t := t - 1;
          s[t].b := s[t].b or s[t + 1].b;
        end;
        52:
        begin{*整型加*}
          t := t - 1;
          s[t].i := s[t].i + s[t + 1].i;
        end;
        53:
        begin{*整型减*}
          t := t - 1;
          s[t].i := s[t].i - s[t + 1].i;
        end;
        54:
        begin{*实型加*}
          t := t - 1;
          s[t].r := s[t].r + s[t + 1].r;
        end;
        55:
        begin{*实型减*}
          t := t - 1;
          s[t].r := s[t].r - s[t + 1].r;
        end;
        56:
        begin{*逻辑与*}
          t := t - 1;
          s[t].b := s[t].b and s[t + 1].b;
        end;
        57:
        begin {*整型乘*}
          t := t - 1;
          s[t].i := s[t].i * s[t + 1].i;
        end;
        58:
        begin{*整型除*}
          t := t - 1;
          if s[t + 1].i = 0 then
            ps := divchk{*标记除零错误*}
          else
            s[t].i := s[t].i div s[t + 1].i;
        end;
        59:
        begin{*整型求余*}
          t := t - 1;
          if s[t + 1].i = 0 then
            ps := divchk {*标记除零错误*}
          else
            s[t].i := s[t].i mod s[t + 1].i;
        end;
      end; {* case *}
    end; {* inter5 *}

    procedure inter6; {*指令操作码为60-63*}
    begin
      case ir.f of
        60:
        begin{*实型乘*}
          t := t - 1;
          s[t].r := s[t].r * s[t + 1].r;
        end;
        61:
        begin{*实型除*}
          t := t - 1;
          s[t].r := s[t].r / s[t + 1].r;
        end;
        62: if eof(prd) then{*readln读完一行*}
            ps := redchk{*标记已读到文件尾*}
          else
            readln;
        63:
        begin{*writeln换行写*}
          writeln(prr); {*输出空行*}
          lncnt := lncnt + 1; {*行数加1*}
          chrcnt := 0; {*行字符计数器置零*}
          if lncnt > linelimit then
            ps := linchk;  {*标记行数超出范围*}
        end
      end {* case *};
    end;
    {* inter6 *}

  begin {* interpret *}
    s[1].i := 0;  {*初始化运行栈*}
    s[2].i := 0;
    s[3].i := -1;
    s[4].i := btab[1].last; {*值为28*}

    display[0] := 0; {*初始化display表*}
    display[1] := 0;
    t := btab[2].vsize - 1; {*栈顶指针t设为全局变量区的最后一个单元*}
    b := 0;
    pc := tab[s[4].i].adr; {*设置指令寄存器的初地址，为主过程的入口地址*}
    lncnt := 0; {*初始化计数器*}
    ocnt := 0;
    chrcnt := 0;
    ps := run; {*初始化指令的执行状态ps*}
    fld[1] := 10; {*初始化默认域宽*}
    fld[2] := 22;
    fld[3] := 10;
    fld[4] := 1;

    repeat {*依次执行指令，直到指令的执行状态ps不为run*}
      ir := code[pc]; {*获取当前pc指向的代码*}
      pc := pc + 1;
      ocnt := ocnt + 1;
      case ir.f div 10 of
        0: inter0;  {*指令操作码为0-9*}
        1: inter1;  {*指令操作码为10-19*}
        2: inter2;  {*指令操作码为20-29*}
        3: inter3;  {*指令操作码为30-39*}
        4: inter4;  {*指令操作码为40-49*}
        5: inter5;  {*指令操作码为50-59*}
        6: inter6;  {*指令操作码为60-63*}
      end; {* case *}
    until ps <> run;

    if ps <> fin then {*当前不为读文件状态*}
    begin
      writeln(prr); {*输出空行*}
      write(prr, ' halt at', pc: 5, ' because of ');
      case ps of  {*打印此时的出错位置和原因*}
        caschk: writeln(prr, 'undefined case');
        divchk: writeln(prr, 'division by 0');
        inxchk: writeln(prr, 'invalid index');
        stkchk: writeln(prr, 'storage overflow');
        linchk: writeln(prr, 'too much output');
        lngchk: writeln(prr, 'line too long');
        redchk: writeln(prr, 'reading past end or file');
      end;
      h1 := b; {*获取此时基地址指针*}
      blkcnt := 10; {*设置blk计数器*}{* post mortem dump *}
      repeat
        writeln(prr); {*输出空行*}
        blkcnt := blkcnt - 1;
        if blkcnt = 0 then
          h1 := 0;
        h2 := s[h1 + 4].i; {*获得过程名在tab表中的位置值*}
        if h1 <> 0 then
          writeln(prr, '', tab[h2].name, 'called at', s[h1 + 1].i: 5);
        h2 := btab[tab[h2].ref].last; {*更新h2，获得该分程序在tab中最后一个字符的位置*}
        while h2 <> 0 do
          with tab[h2] do
          begin
            if obj = vvariable then
              if typ in stantyps then
              begin
                write(prr, '', name, '='); {*输出变量名*}
                if normal then
                  h3 := h1 + adr {*对于非变量形参取其地址*}
                else
                  h3 := s[h1 + adr].i; {*对于变量形参取其值*}
                case typ of {*输出变量值*}
                  ints: writeln(prr, s[h3].i);
                  reals: writeln(prr, s[h3].r);
                  bools: if s[h3].b
                    then
                      writeln(prr, 'true')
                    else
                      writeln(prr, 'false');
                  chars: writeln(prr, chr(s[h3].i mod 64))
                end;
              end;
            h2 := link; {*更新为同一分程序中上一个标识符在tab的位置*}
          end;
        h1 := s[h1 + 3].i {*更新h1为当前分程序的动态链接*}
      until h1 < 0;
    end;
    writeln(prr); {*输出空行*}
    writeln(prr, ocnt, ' steps');
  end; {* interpret *}



  procedure setup; {*建立保留字表key和ksy，特定字符表sps*}
  begin
    key[1] := 'and       ';
    key[2] := 'array     ';
    key[3] := 'begin     ';
    key[4] := 'case      ';
    key[5] := 'const     ';
    key[6] := 'div       ';
    key[7] := 'do        ';
    key[8] := 'downto    ';
    key[9] := 'else      ';
    key[10] := 'end       ';
    key[11] := 'for       ';
    key[12] := 'function  ';
    key[13] := 'if        ';
    key[14] := 'mod       ';
    key[15] := 'not       ';
    key[16] := 'of        ';
    key[17] := 'or        ';
    key[18] := 'procedure ';
    key[19] := 'program   ';
    key[20] := 'record    ';
    key[21] := 'repeat    ';
    key[22] := 'then      ';
    key[23] := 'to        ';
    key[24] := 'type      ';
    key[25] := 'until     ';
    key[26] := 'var       ';
    key[27] := 'while     ';

    ksy[1] := andsy;
    ksy[2] := arraysy;
    ksy[3] := beginsy;
    ksy[4] := casesy;
    ksy[5] := constsy;
    ksy[6] := idiv;
    ksy[7] := dosy;
    ksy[8] := downtosy;
    ksy[9] := elsesy;
    ksy[10] := endsy;
    ksy[11] := forsy;
    ksy[12] := funcsy;
    ksy[13] := ifsy;
    ksy[14] := imod;
    ksy[15] := notsy;
    ksy[16] := ofsy;
    ksy[17] := orsy;
    ksy[18] := procsy;
    ksy[19] := programsy;
    ksy[20] := recordsy;
    ksy[21] := repeatsy;
    ksy[22] := thensy;
    ksy[23] := tosy;
    ksy[24] := typesy;
    ksy[25] := untilsy;
    ksy[26] := varsy;
    ksy[27] := whilesy;


    sps['+'] := plus;
    sps['-'] := minus;
    sps['*'] := times;
    sps['/'] := rdiv;
    sps['('] := lparent;
    sps[')'] := rparent;
    sps['='] := eql;
    sps[','] := comma;
    sps['['] := lbrack;
    sps[']'] := rbrack;
    sps[''''] := neq;
    sps['!'] := andsy;
    sps[';'] := semicolon;
  end {* setup *};

procedure enterids; {*在符号表中登录基本类型、函数和过程的名字，以及它们的相应信息*}
  begin
    {*把各个标准类型、过程和函数的名字依次登录到符号表tab中*}
    enter('          ', vvariable, notyp, 0); {* sentinel *}
    enter('false     ', konstant, bools, 0);
    enter('true      ', konstant, bools, 1);
    enter('real      ', typel, reals, 1);
    enter('char      ', typel, chars, 1);
    enter('boolean   ', typel, bools, 1);
    enter('integer   ', typel, ints, 1);
    enter('abs       ', funktion, reals, 0);
    enter('sqr       ', funktion, reals, 2);
    enter('odd       ', funktion, bools, 4);
    enter('chr       ', funktion, chars, 5);
    enter('ord       ', funktion, ints, 6);
    enter('succ      ', funktion, chars, 7);
    enter('pred      ', funktion, chars, 8);
    enter('round     ', funktion, ints, 9);
    enter('trunc     ', funktion, ints, 10);
    enter('sin       ', funktion, reals, 11);
    enter('cos       ', funktion, reals, 12);
    enter('exp       ', funktion, reals, 13);
    enter('ln        ', funktion, reals, 14);
    enter('sqrt      ', funktion, reals, 15);
    enter('arctan    ', funktion, reals, 16);
    enter('eof       ', funktion, bools, 17);
    enter('eoln      ', funktion, bools, 18);
    enter('read      ', prozedure, notyp, 1);
    enter('readln    ', prozedure, notyp, 2);
    enter('write     ', prozedure, notyp, 3);
    enter('writeln', prozedure, notyp, 4);
    enter('          ', prozedure, notyp, 0); {*把主过程登记在符号表中*}
  end;


begin  {* main *}
  setup; {*初始化保留字数组key和ksy，特殊符号数组sps*}

  {*依次初始化常量初始符号集合、类型初始符号集合、分程序初始符号集合、因子初始符号集合、语句初始符号集合*}
  constbegsys := [plus, minus, intcon, realcon, charcon, ident];
  typebegsys := [ident, arraysy, recordsy];
  blockbegsys := [constsy, typesy, varsy, procsy, funcsy, beginsy];
  facbegsys := [intcon, realcon, charcon, ident, lparent, notsy];
  statbegsys := [beginsy, ifsy, whilesy, repeatsy, forsy, casesy];

  stantyps := [notyp, ints, reals, bools, chars]; {*初始化基本数据类型集合*}

  lc := 0; {*初始化P代码表索引lc*}
  ll := 0; {*初始化当前行长度*}
  cc := 0; {*初始化当前行字符计数器*}
  ch := ' '; {*初始化当前字符(从源程序中读取的最后一个符号)*}
  errpos := 0; {*初始化编译错误位置指针*}
  errs := []; {*初始化编译错误集合*}
  writeln('NOTE input/output for users program is console : ');
  writeln;
  write('Source input file ?');
  readln(inf); {*读入源代码文件名*}
  assign(psin, inf); {*文件名与文件变量psin关联*}
  reset(psin); {*打开源文件，指向文件元素的指针指向第一个元素*}
  write('Source listing file ?');
  readln(outf); {*读入编译输出文件名*}
  assign(psout, outf); {*文件名与文件变量psout关联*}
  rewrite(psout); {*建立并打开输出文件，指向文件元素的指针指向第一个元素*}
  assign(prd, 'con'); {*文件名'con'与文件变量prd关联，作为输入文件*}
  write('result file : ');
  readln(fprr); {*读入结果文件名*}
  assign(prr, fprr); {*文件名与文件变量prr关联*}
  reset(prd); {*打开文件prd，指向文件元素的指针指向第一个元素*}
  rewrite(prr); {*建立并打开文件prr，指向文件元素的指针指向第一个元素*}

  {*分别初始化tab表，atab表，btab表，stab表，rconst表的索引*}
  t := -1;
  a := 0;
  b := 1;
  sx := 0;
  c2 := 0;

  display[0] := 1; {*初始化display表*}

  {*初始化各标签flag值*}
  iflag := false;
  oflag := false;
  skipflag := false;
  prtables := false;
  stackdump := false;

  insymbol; {*读取下一单词符号，处理注释行*}

  if sy <> programsy then
    error(3) {*第一个符号不为program保留字时报错*}
  else
  begin
    insymbol; {*读取下一单词符号，处理注释行*}
    if sy <> ident then
      error(2) {*符号不为标识符时报错*}
    else
    begin
      progname := id; {*获得程序名*}
      insymbol; {*读取下一单词符号，处理注释行*}
      if sy <> lparent then
        error(9)  {*符号不为左括号时报错*}
      else
        repeat
          insymbol; {*读取下一单词符号，处理注释行*}
          if sy <> ident then
            error(2) {*符号不为标识符时报错*}
          else
          begin {*判断标准输入输出关键字，设置对应的flag*}
            if id = 'input     ' then
              iflag := true
            else if id =
              'output    ' then
              oflag := true
            else
              error(0);  {*出现非输入输出关键字时报错*}
            insymbol; {*读取下一单词符号，处理注释行*}
          end
        until sy <> comma; {*重复此过程直到读到非逗号符号*}
      if sy = rparent then
        insymbol {*当前符号为又括号时继续读取单词符号，否则报错*}
      else
        error(4);
      if not oflag then
        error(20); {*参数中没有output关键字时也报错*}
    end;
  end;
  enterids; {*在符号表中登录基本类型、函数和过程的名字，以及它们的相应信息*}
  with btab[1] do {*设置btab[1]，初始化分程序表*}
  begin
    last := t; {*last指向分程序当前最后一个标识符在tab表中的位置*}
    lastpar := 1; {*lastpar指向过程或函数的最后一个参数在tab表中的位置；*}
    psize := 0;
    vsize := 0;
  end;
  block(blockbegsys + statbegsys, false, 1); {*分析处理分程序*}
  {*分程序初始符号集合和表达式初始符号集合*}

  if sy <> period then
    error(2); {*程序结尾不是点号是报错*}
  emit(31);  {*生成停止指令*}
  if prtables then
    printtables; {*打印编译生成的符号表、分程序表、实常数表以及p代码*}
  if errs = [] then
    interpret {*无错误时开始解释执行程序*}
  else
  begin
    writeln(psout); {*在编译输出文件中输出空行*}
    writeln(psout, 'compiled with errors');
    writeln(psout); {*在编译输出文件中输出空行*}
    errormsg; {*打印程序中出错信息的摘要*}
  end;
  writeln(psout); {*在编译输出文件中输出空行*}
  close(psout); {*关闭psout文件*}
  close(prr); {*关闭prr文件*}
end.