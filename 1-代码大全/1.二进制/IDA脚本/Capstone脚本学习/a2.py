from capstone import Cs
from capstone import *
import struct

def decode(offset,tmp):
    data_bin = open(r'E:\QQsavefile\MobileFile\mazes.exe', 'rb').read()#exe路径
    data = data_bin[offset: offset+10921]
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    inscnt = 0
    inscnt2 = 0
    map1 = []
    map2 = []
    map3 = []
    for i in md.disasm(data, offset):
        ins = "0x%x:\t%s\t%s" % (i.address, i.mnemonic, i.op_str)
        if '\tmov\tdword ptr [rsp' in ins or '\tmov\teax, dword ptr [rip' in ins:
            if inscnt < 625:
                #print(ins)
                map1.append(int(i.op_str.split(', ')[1], 16))
                inscnt += 1
            elif inscnt2 < 2:
                map3.append(int(i.op_str.split(', ')[1], 16))
                inscnt2 += 1 
            elif inscnt<629:
                off1 =i.address+int(i.op_str.split(', ')[1].split('+')[1].replace(']',''),16)
                map1_data = data_bin[0xa76098+tmp*5: 0xa76098+4+tmp*5 ]           
                map1=map1+(list(map1_data))
                inscnt+=4
        if '\tlea\trcx, [rip' in ins:
            map2_data = data_bin[tmp*2512+0xa82610: tmp*2512+0xa82610+ 4 * 625]#base=off1-(0xa8112c+i.address)+0xa82fe0
            for i in range(625):
                map2.append(struct.unpack("I", map2_data[i * 4: i * 4 + 4])[0])

    data = []
    for i in range(625):
        data.append(map1[i] ^ map2[i])
    return data, bytearray(map1[-4:]), map3


def checkValid(map, x, y):
    if x < 0 or y < 0 or x > 24 or y > 24:
        return False
    return map[y * 25 + x] == 0xd6


def solve(map, startX, startY, direct, path):
    map[startY * 25 + startX] = ord('*')
    if len(path) == 15:
        return True, path

    all_dir = []
    if checkValid(map, startX - 1, startY):
        all_dir.append((startX-1, startY, direct[1]))
    if checkValid(map, startX+ 1, startY ):
        all_dir.append((startX+ 1, startY , direct[2]))
    if checkValid(map, startX , startY- 1):
        all_dir.append((startX , startY- 1, direct[3]))
    if checkValid(map, startX, startY + 1):
        all_dir.append((startX, startY+1, direct[4]))

    for dir in all_dir:
        result = solve(map, dir[0], dir[1], direct, path + dir[2])
        if result[0] == True:
            return result
    return False, ''

def printMap(map1):
    for i in range(25):
        line = ''
        for j in range(25):
            line += chr(map1[i * 25 + j])
        print(line)

total = ''
tmp=0
for i in range(0,10933056+10944,10944):
    map1, dirs, target = decode(i+0x860,tmp)
    tmp+=1
    # print("process:" + hex(i+0x860-0x78a))
    rr = solve(map1, target[1], target[0], 'A' + dirs.decode('utf-8'), '')
    total += rr[1]


import hashlib
mm = hashlib.md5(total.encode('utf-8')).hexdigest()
with open("1.txt",'w') as f:
	f.write(total)#ktkktttAtAA3AAA]]XX>>XXXX>XXX>A9A9AA--e-e--AA
print(len(total))
# total="ktkktttAtAA3AAA]]XX>>XXXX>XXX>A9A9AA--e-e--AAL.LL$$6$$$$LL$$5$$$$$@$@$@$$@@p<ppp8888pp8pppm[[11[11C11[11Ci@i@iizzzii@iiz??r???::??:?:::zooo55&&5&&55&&nn(nn(n(nnnnuuu`'`'``''e''`''eQ===jjIIjj==j==7qqq7qqqOqqqOOqzXzXzz,zzz,z,,whhhhZZ^Z^^Z^^^Z(ii]ii(((iii]i]oMMMLMLLTTLLTTLrzzzIzzzrzzIIzzR$$$++$+$$$$R$RXXHHHhhHhHHHhhH]]']''ZZ'ZZZIZI9*99BBmBBmm*mmB#NNaNaa aaNaaaNYSSYYPPYPPPPPP5L1L11Z111L11ZZZ4 44?4?4??$$ $$bWbbWWWbbWbWbWWU55===55=5==ooo#ZZZ#Z#Z#ZZZZSS&&&222YYYvYvYvvz>zzvzzzzvzzvzz))e)e))P)PPPPSS33MMM33u33uuuju[>[[[>[[i[[>>[[wwww+wwhh`h````--x-x--x--  T TvHvHvHHooToTooTv]]]Z]]Z]]]]]v]WnnWnWnnWWWWW--++]+]]++]++3++]QKQKKQKK=K=KKK=KkKkkkpp7p77p77hhhhhhhshhshsss! !  c  !!l!!!!xWxWWxx<x<xxxx<7F77T77TTTTTTAA,-,,--[-[[[[-[-B1BBBB1BB1BB1B13AAAAAAOOCCOOOC?v?vvvv???,?,??lDDDlDDDDlDlDll+cc+c+++VVVYVV+;&&&;&&&&;&;;&&qcqqqqfqffqqcccbGG333GGGbbGGbG8jj8j88jj(j((j(##>#>##C#C####>ffcffcfffff&f&&VV,VV,,VV######$$&$&$&$&$$$]]]jjPjjjPPPP::b:b!Y!Y!Y!YYOYYOYO`h`hh  hhh`hh h;;;;;;66;666|66[WW55a5a55a5aa[,zzz,,.,..F.F..w1w1w1w1w1111wwWW===VV===VVNVVIpp ppIII666IIIOOAOAOAOAOOLOLO:   QQ  QQ QQoQ66P6P666PP666b6gogoggogoo1o11O[1[[[H[[H[HHHHHH3H3H3HH3H3HHHH99q99::3:3:3::3^^YY66YY^YY^^Y^h!!!!RR9R999RR!*j*j***JJ*JJ*JJ:???:??::?:::FFyee<e<e<eeyee<e2&&&2&&&&yy11y1yCCC:CCyCC::C::UkUU))x)xx)x)x)[_[[YY[YYmYmm_mbkbbwbwwmmwmmmwI2I2I2IIMIIMMII'a'a'aa`aa'aaaa~TTjTjjMMMMjMjj:_______q__q_qq99$99$$$-$$99$$aa>a>>>>>,>,>,>==f===?===ff=f=ruruummDmDmmmmuHOHOOHHOHHOHOHHPXPPPGPGPGPPPXXcZcckccZccZcZZ[QQ*QQQQQSSQQ***NuNNNNNCCNNuNuuRRRRRlRll&&ll&&>,,!,,!,!!!,,,!D@DDDDD3DD3DD3Da1a111a11Z1Z1ZZd2ddd2d22PPBPPPZ''Q'''Z'Z''Z'Z]&]&&&&K&KKK~~K<g<<<gg<<<^<^<<'K'K'KKKKKYYKK'zTTTTTTqqTqq7qq!f!ffHfHHff!ff!CCAACACCACC6CCC1>>1>>11^111^1^nMM&&&&&&&ZZnnZVgVVV99e9eeee99p77pppppp00----&3&3&3&&&KKKK&KaXX3XXXaaUaaXX344ggCgg44g4gg44DLDDrDDDDDDLDLDhphphpphhpp6pp6K$$r$rrr$r$$K$Kd$$ddVVVdVddVVVYHYY)))Y))YYHHHfxfxfxxfxffvfvv!CC]]`]]``]]```6ddd66||6|6||||><<><<<p<p<p<pp(B(BBBB=B=BB===ncccnccc[c[c[[cYVV]]!]!!!YYYY!fNNNNfNffJJJfff+>>>++>>++>++>+FiiFiioo3o33oo3RR[[[^^[^^EE^EElkkk]]]kkklllll[4[44mmmm4mm444v==v=====77PP7P_W_WWQQBQBQQBBB??t?????ttututt900+0+009099999QQRQQpQpppHHHHRlFlFllxlxlxxx>>CCFCFFCFCCC888txWxxyxyxxxxWWxxklkklllllllllkkP22ll'l''l'''P'OO11]1]1]]]]hhhHHH7H7H7HHHH3HH  JJJ)J)JJ   JJonoo4oonnononon;^^;^^^e^eee[eeEE|||k||EE5E5EEdtttrttttdtddtdfpfffMMMMMfMMJM*___*__**>>*>>*>C>>>222H2H22>>-t--]]&]]&]]&&t!qqLL8L88!88L88HNHNHHH''KK''KK'O''KKIKIKKKKIK66y66>6>66>>66yVVVaaVVVVVgghggE--[[---[-[[[[-EEE==c=ccc===E=[[q[[[EECCECECEn99n9999E99n99EcIIccIIcc]]c]]]`P```P``v``P``vDDD|||gg||D||||`a``a`a`a`aa+++o88e8e888e8e88owccwcccc2c2c22cqqq;;qq;;;;!!;;lHHHlllllHHHPP h77hh[[h[hh[h[hpp`p```Y`Y`YY``^R^^S^^R^R^^RRR|[[|[||QQgQQ|||'ddd**44**4**ddQQQ@@ssssTsss@s qq  q qqlqq q ODDOOODOODDODDOv$v$$vvxxxXxXxxJlJlllJlJll$$$I?W????55CCC5555e<e<eeeJJiiJiJJ]7]]]]x]x]]77S7444I443344I4444-AAARAARARRRAAAjjFjFjjsjjjjjsjZUUkUkUkUUZUUZU>>>*>*>>S>>>*>>FFF......<..F..++r+r++r++rrXrXKKNKNNKKKNNN6NN'666jjj0000'0''llliillll55b5b59mm99(9(99(9((9sszszz```E``E`E1    1 11e11 1 AAAEAAAAAEEAEAAVZVVZZZVVZVZVV^A_A_AAzAzAzAAzA0000f000f0fff8fxx@@@!@@@xx@xxx~'~''~''~''OO]Ozz_z_z_zzz_z__Okukukukuukk111k5!!J!!!5!5!5!5!11WWUWW1WW1WWUWR11111RRR1RRERECCjjwwwjjCjCjjj@@K@@@000@@KK@KEEdEEEgEgg#gggE<II<<<I<IIjIjIj;;nnnn;;nnnnnnnvvyvyyvvfvffaff2VVV2VVVUVVUUVVLLFLFF&FFLFFLFFAAVVVGVGGGVGVVV((wwwjwjjwjjjjj*$$zz:zz$$zz$$zfJfJJHHHJHHHH'H2OO2OOO@O@@___@77yy77y7yyddfddQQiQQQiiQiQQQQM(hhh  hh   N N QQfQQQfQQAQQfQQY|YYAAvAAYAAAvAyNN]]]**]]**yy*oFF===F===,=,,=~~SSDDDuDuu~u~uDVDDDsDDVDDVVVD''r'''9'''''r'rtt''K''tt''tttEyRRyycycyyyRRyyx,,,XX,,x,,,x,,)))|)|))|||||)|?SSRRRSS?SSRRSSAA222266222AASAY4Y4444yyEyEyyyfnnrr_r_r_r_r_r<s<<<44mm444<<<FBBB,,b,bbbFbbFWVVVVVWVVvvvVVW9eeedd4d44d4494&&Q&Q&&iiii&ii+QQ77QQQQ$Q$$[$[buuFuFuFF7FF7F7II;IIIII;IIXIXXPPPP<<P<PPP_PP_TCCTCTTCTTEEEEEFAFFAAFFAFAFFFFzKzKKzKzz<z<<!!r''ttFtFFrFrFFr!!II!!I!I!!!III!!!``!!`!!!!`!`EEEEKEKKKEKEKKK|)))PPwwPwwwww|[q[q[q[qq[qq[qqFF***FF*F*F*F*F]99c99cc99999]]jjkjkkjkkYYOYYOsussss((ss(s((sZZSZSZZxZZSSSZZPYYPYYPYPPYPP3PjjjHjHjjHHHjHjHLLttLtLLtLtLtLtrrCrCCrCCrrr$r$mmm___mmmaammmaUVUVVUVVV>>JJ>>~~X~X~XXX4X4XX4cwwccvcvvcvv(((<uuuooAAoAoouoo88FFFMMFFF88f8f<nn~n~nn<n<<nnn&&PP&PP&&&4&444RRRBBRBRBBRBRRB5]]]]]]N]]]]]5]|YY||||Y|YYVVVVUUOUUU5U5U5U555zzzz^^z^z^^L^L^4ttt772277t7tt7$s$s$$HHpHH$H$$cgcchcccccgccchIhIhh#h###h#hhhaTaTaTT!!U!!!!!(ss(ssttsssttjtAA777AAAA777A77))))))Z)ZZYY0YYO##OO#O#O###&&#KbKK]]K]K]]]KK]CHHmmNmNmmmmHmm)ZZ0000000ZZZ0ZZ77ZZ--ZZ--K--Z33ee33Q3Q3QQQQ3xx((z((zzOzzOzzJJJJJJHHHJHHJHJ3J333;33JJJJGGGC,CCyCC,CCyCC,,HHHHoototoooootLL++V++LLLLL6LL??#?#?###(((44(I``w`www````wwZEvvvEE|||E|||E|***ccNNcNN~N~NN$$$z$zzzz$zz$$*Q''nnGnGnnnnnnnooBooBBBByByyWW33]]ddd]dd]d]]3)))d)d))Y)YYY)Y77t7t77722722EEAA[A[AAYAYAAYAY _  __1__111y11,^,,R,,^,,R,,^,FmFmFFFFFIFIFIFQQsssQsQssQsQQmXX`X`X`XX``88``IIwIIwwIwwwIIyI Q  yy~yy~y~y~yjj@@88@88@@88w8U``~`~`~~~`~~`~HVVVHHVVHHLLHHLtBBB77B7B7BBBttA0A0A000AA???AAJ@JJJ@@EEEE@E@Ez_zz11z11z11111qq--qqsssqqsssJnRReRRRReeesessUKKUUU33UUKKKKK><<<>>>q>qq>qqqO@O@@O@@OO;;;I;m;m;;BB;;BBB;B;RRSSaSaa1aa1aa1]]]ooooggWggWWgbpbb~b~bb~b~~~bZCCcCCCCZZHZZCC(4(((444]] ]]4]]8]8]]!!4!!44!!8888__8___88_88XXfXfXfXXttttDDZ44<<44Z44Z444<>>i>>H>H>>>>HH>qqCCkkkkCkCCCCCgg,g,g,gg,g,,&&UUBBBeBeBeBBeBB4pp44###~#~#~#~#<##???b?bbb<bbuu:u:uu]]h]]uu:..RRR]]]]RR..RRlfllflffllflflfw5w5w55w5wwfwfweeqq,,q,,,,q,,qE&&RRRR|R|R|RR&olllrrqqrrrrlll==XX==00==XX=X=qSSSSSqSSSSqSqq2WW2W2W22QQSQQQ2ggg``gg2g2g22HY4YYjjfjffff4ff;GGG8GGGG88`888666~~F~~~F~F~~Fummkmmmkkkmkmkmm]]@@]@@@22mm22>nnGnGnnn>>>>>n;,;;,,;;XXXXJXJvRRRNNRRvvvvv..EE...EEE.EE==E=~jjjjjj~~ooooooQQvQQQ<<A<A<<AAllBB!B!!!+!++l+RRRqqqqqqRRlRRl22MMGGMGMMGGMM2'nnnSSSSySSSyy'[#[##P##[###[[Z00!!!0!00HHH,HH'Z''F'F'''Z'Z''llZZlllss's'ssls)))````<```<<suguuggTTgTToTTom~~~mm~m~mmgmm~]II]]]]rrr]]]I]aCCDDCDDCDDDzDDW99mm99WW9W999mAkk|kk|kkkk||xxhrrOOOrOOrrhrhrOOOrOOyyyyywwrw00*00**000+0+00wiwwiiBiiBiBiBi3MMM`MM```M`M`M'6'']']']]]RRR]5'''q'qq&qq&&q&gOggssssgsgsgssss`s```??!!???!SwSwSwSSS@@hh@@HHpHppiiiiiuiui!1!!!111111i1iiSbb~~~M~M~~M~M~6666886666996699mmaa7aamaa77aavvzznnzzvzvzvzv##Z##ZZZhZhh---e|ee55&5&5&55eeJgJJgJJJJJJJJxxZ<<ggBgBgggg<g<*mmm'mm'm'mm*m*MvMvMMvvMMMMttCBPPzPPPBPBBPPPBI2II22II2III22z aaaaaaaaaaLaLLT*TT+TT+T+T++kkX8X8XX8XXX&XX88=||55A55A5555AA&&&&CCvvvvvCCCCMMhhhhhshhssaasssMMMsszzszzsszy>y>>y>>0>>00>>]::]]GGGG]G]GGS#77(7(((|(||#||c&c&c&c&&<<.<<<e4ee44cc4444e4e###kkk##FF#FFFFVVV-V-VV--c--c-JJmJJEE`E`````E((mmddddmmmdddd|R|R|RR|R||C|||zezzzppKKpKppKKoSoooxoooSoSoSSkkekeeekk5kkeeef,,7,,,,ff9ff,,aSaanannnn555n5oQQQooQooDD>DD>WWW;;****Z**ZZW8[88[88ccWccWW[p???WWWZWW?W??p3RR++++H+HHHHHH))ddddddddddxxZ:::AAAA--f-----`#````_```__^^_+a+a+aa+++a++H+WqWqWqWW$WWWqWW'Z''__''_'_''ZZHHHHooHHPPPHPPHhh[[@@@@[@@[@@>hmmm===m====m==qUqqqqqHHqHqHHHxxuxuxuuxxuxxxxxppppwwwwwwiiiicKccKKKKccKcKcK[][][][]]]``]``xbbwbwwbwwwbbwwWW,WWW,,,M,,,WWccRRRcc<<cc<c<cdMdMM||,|||,||,8P8P8PPePP88PPe:8:88:8::33Y33:pppp]ppQQQQQpQQ)))00000;0;0;0;m@m@m@mmmFFmFFFAAxAA^^AAAAA^A^7EE777Z7ZZ777E7XXaXXaa,,,a,,aaq|qqIqq||qq|||LExEEEQEQQEEExEExxyyxxyxx&&&&&S[>[>>[>[[;;d;;;]tt]]]t]t]]B]]tyyPP3333aaayaa3!!eeEeEee!e!e!exyyyxyxyyxxxQxQtttttt&&tt>t>>>PsPss@sssssPss@DDoouuoouu||u|u&UU''5''555''55Z0ZZZZ00Z00M00Ma=aa=a=aa=aaaa,;*;;**_*_**__|_`|`` `   &&&&|&EE==E====E=EE==k55J55JJJVVVJJ5'QQsQQsQQ'''O'O_HHHHVVV]]VV]]]JJ33N3NNqqNNqqNGooopooGoooGGNN tt t      tttVB!!B!B!B!BB!!B!535533eee3e3eeeuu))```m``mmummallall   lll   88<<mmm0mm<<8<<DkDkkuu;uu;;uukYKYYKKxxKKYKKYKA`A`A`A`A`A``r`[[[_[_[[CCKCKCKCQCCQQ00Q0QQCCQcczzzggzggzgzgz77b77700007777bppFpp5p55pp55-5JmJJmmmJJLLJJmJ0nnWnnn000n0nnn&g&g&gg&&rrrSrSZZzzzZzzzGGGaaa)SS)S))hhhhh)h)5CCCYCCCYYY444Y3D3D3DD(DDDD(D(~UU*U***99****9KzzLzzzzKzzKzzzZUZZRRZRRRR]RR]O0O0O00O0O000O0uuu@@)@@@@)@@)@G$$GGGkkGG$$G$$tt:ttt::P::t::PGGnn+nnGG>G>GGn33?3??RR?RRROO3u3u3u33bbbb===b)]]9]9]999]]9]]-ff77g7g7g777g7ypp==!=!!!=!!!=u++uu#u##z#z#zz$@$$$oo$oo$$$oonn..nnNNnNnnNnN((q(qq))q)qq(((fJff5ffJfff555(qTqTTT&TTqqqqTTsaassrsrsrrssrr(-(--1----1--((&&&||&|&||w|wwwWgWggggggWgg~~~o~~w~wwwNwNNoNNyWW-W-----11y11sfsfsffcfccfcffGGQGQQQ`Q``QQQQRR>RRBBqBqBqqBBMMsMMMssMMWMMsMvv9v9v9v99vvzzz:H:HHvvHH:H:HH:hfff000000f0ff04-4--444XX5X5X5ffffuuff666=6666j66jj|||.|.|..cc$$;;;:;;;;$;$272272727227722&.&..&..??..&&.CCdCCCCdCddddddPPPPPvPP)PPPPvvW##1#11m1mmWmmm99w99!!9!9!!HHHWWW....(.(...(($&&ssVssVVss&s&u??i?ii???u?uuuPvvvAvAAvAvvPvv+J+JJ+JJJJ--JJ-PZPZZZZZZZhhZZh##hh&hh##h#####TggTgg<<U<<gg<g|E|E|||||E|E||Ljjj@jjj:jj@@@dds@@g@g@gg4444gg55))5)))))EEE|E(KKPKKKPKKPPPPK$jj$$jjj$$$j$$$KNNKNNllNlllNlNLOOOOLLiiiLLOLL,,.....8.8..,..QQQQQZZ111111C1ee11ee1e1e1e111XXpppXppeeepeeptt4tttTTtttttTtxII#IIxIIxIxIxxrrxxiiixiiMMiix##ooAoo#o###GG#,,,,M,M,,M,,x,x HH++8+88 888++FFJJ>JJFFF''FFFtvtvtvttyyyyoyyddd99d9999dd~~~rVrVV8898988989+u++99T99T99999-SSSySySyy[[yySgIgIIggIgIIIeee||u||uu||44|4|4@w@wwgwggwwgwggVfVVDDVVffVVVDDQvQQQvQvv|v|v|v_x____A_AA__x__mGGmGmmamamaa1aR!!!!QQ!QQEEEQE]]R]RRRRRR]]]]RGkGkknnnnkkGGkku`uuuu``7`7```uvbvbvvbbbbvbbrr,ss,,Z,Z,,ZZ,Z,grrgrrrgg8ggrrEPAPPYYYuYYPYPPPNcNNcNNQNNNcNNN`&`&``?`?```?``|D|DDD||DDRDRRRZZCCZCZCCC;CCZZX^X^X^XX___X___5QQ55555q55Q55q[JJJJ[JJJ[[lllllHllYYYmYYlYlYl2VV2V2VVV2V22))jjrrjrrjjrjrrjjZZrZZZrrrZZrrzz??ttt?tt??tteeeq@@q@qqSqqSSIISxVxVxxxxVxVxVxVtmmtmtmtmmmttmmj777j77__Y_Y_YYCCiCCDDCDDHHiHHwLwLLL,,LLLwLwwgbggg'g'g'gg''^uuSuuSSuSuSuu<ui@@@@z@z@zzz@zz   X XXss9ssXX @ @  @   @@@ @@k<<ccucccc<<ccu5E5575777077007|W|W||WW||99||9wqqq>>>``>`>```vvv++v++d+dddd+;U;;!;!;;U;U;UU****kkokookoook2##2###xx#####2[[##1#1#1#1111*F5555[[[;[;;F;;hAhhhBBBBhBhhAAI****BBBXBXXII*fnfnnnttnttnttX!!.!..V.VVV..V.eXXLLLxxxLxxLLX!!O!!O!OOOwOwOw9c99c9cctctt]]9^hhhhhddhdhh^hhxNNNmmmHHHHxHHmooowoowwoo^oowwNNNbb  bbNNbbNbapapappppp11p1144h44hhh4466644zFzFFzzFzz@z@@2**000444400400*:3::P:P:PPKPKPK))))),))444)4))998988998991191SMSMSSSrSSMSSSMduududuuuuddidd[[[[bbb[[[[b[[z,,,,00N0NNN0NNN.t..tttwwtwwcc.;AAA;;W;WW|W|WWxxFxFFFxFFFFFFezffeffzzfffee_e2jjOjjj2jjOjj2jq1111h1h1h11hh3bbRbRbbRbb))b))X[[BBB0000000XX!R!!B!!BBZZZZZZ--7--88.8..8.8.]f]]]n]]ffffbbny]yyy]yy]]++]++YYY;YY*YY;;Y;Y;,,pp$pp,pp$$+$$[uuIIuuu[[a[[[uC[[CCCVVPVVVCVV5O55OO55O5555B5]dddBBBdd]]d]]]00B0B00B0000B0BuWWW7WWWWuuWuuuyyZyZyZyZyZyyyZSS~~44~4~4~4~~SAAA44nn44n4n44nDDKDKDKDKDKKDDK!II!!VVV=VV==V=LL-LLWWWNNNNNWWU)UUU))UUTTUUTTbbqqqqbqbqqbbqqxYYxYxx_x_x__x_'s'''ss1ss''F'FCC&CCRCRRhhRhhhNNzzNzNNzNzzYYzLeLeLeeLLgggvgvh$hhhhh$$33333$:bbfbffbfff(ffbJJbJJbbJbJJJoJo;V;V;;VVAAAVAVA*KKKKAAAAKKKAKK]]U]]UU??U??t??11@@33@@1@11@1@MMiMMMiMiMiiiMiHHHHH4HH4H4HH44PPrPrrrPrPPPPrP=cc=cc==cc=cc==|ZZUUUUUOUOUUUO--:--**----:--:aRaaRRYYYRYRRRR;;XX(XX;XX((X((PUPP+++P+PPUUPUFnFFNNNFNNNFNFFe)))eeeeieii__iDMMM...00.0.00.f''000XXXX00''0Yss*s**_*_**ssYXpp0p0pp00070771@11zz1zzzz1111<22<22<2<2222<23G33GG3G3GG3333egeggffffffggggddTdTddddddWWEExxxDxx))V)VVV))$$AAjAjAAA$A$AAAAAAiAiiiii,ii,5tt8tt8ttt8t88t00d0d0dd7dd7d77LQQLQQLLLQQLQLQZZ22ZZDDZZ2ZZZD$88$8888r88r888q''''q''q'''q''VVVVpp3ppVpVppVTcccdcccddcccdcvxvvvvvxxxxxxxxhYhhYhhh55SS55h,,JJ,,JJJRRJJJJ-22-----2-22-2-avv[v[v[[d[d[[[r5rr55555r55r55C$C$C$C$CC$$C$$MnMnnMMMMnMnnnMpIIpp^^^n^nn^^n#Z#Z#Z###|##|#|wwwFFwFFaFFaFaa;E;;;E;;E;EE$EELMMM,,MM,M,MMLLuuuuRRuRuuUuuUuYYbYYbbCCCGCCGC~---@--~~i~iii~gOgOOLOOOOgggOOL***LL*L**L*LLL--u-u-u-u--n--u3w3303033wwNww3VVVxVVxVxVxxxxxvTTT!!T!!!!---!Q..AAEAA..AA..ALccc2cc2ccLcLccFEFFEFEEFF(FF(FI0I0000!0000!00rjrrr'r'r'r''e'eyeeFF|F||FFFFefttPtPttftff5ffXX6X666x66X66XXRGGG`GGGRGG```G77]]7]]]7]777<7EYYYYYxYxYYEYEY(((22p22(2(((KKp3334433ppppp3p^E^E^EE^E^^^E^^_b_b___b_bb___-|???nnniiininin>k>>4>4>>kkckkkRrRRRrRrrRrRRrraaa333<<!<<3<336XX6666W6WWppppTPPvvwvwvvwvwvw.RR.R..R..R..1.qffkffqffqfffff [[uutuuu[uuuuuii]]i]i]]q]]i]]`c``c`cccccc`cc0!00`00!00!!!dd2<2<222rrTrr222>>ff>>>ffafaaTa#OOO###22#2211OW]W]];];]];;;;]j>j>jj>>V>VVVV74Y4Y44@44@@ @ @1411__|__1_1144((JJwJwJJ((p((pECC...C.CCCEEEC2ii#i#i##&##iiiC99]]H]HHH]H]H]OOqOO[O[[[9999[ssqsqss_sss_sss0*008000**k****@@@C@@@CCgggCCC*u*u*u*u***A*AA9=99@@N@NNNNN=N03030303322D22Dgyygygyggyygygg7SSYSYSSSSS7SS7)S)))q)qq==q==qgHgg~~gg~ggHgHgoToTooTTTooTTTT5N55N55K5KKOOKKDssOsOOsOsssDDstKKtttKKttt((tt!pp!p!p!!(!(!(!KpppKpppYYppKKKl,l,l,ll,l,llHHWQWQQQQ;QQQ;;QQ^^^>>^>>^^eeeNe,!!``!``!`!!,!,0===PP===00_0_0n;;nnSSnnSSSSSSV((V(((y(y(yyFF-11-1--::C:::-:N^N^NN^NNN^N^^YjvvjjjvvvvXvXvvvHHnHnnHHHnn!!nUoUUUUsUsUUsssUJ[[i[[[J[[JJ[J[99999I9I99z99zz>>5>>5>555>>WWWQQ^^JJJJ^^Q^QQQoaa++++a+a++aaa~~,~,,~,,,~~~,~r0rr00r0r0r00rr8888j8j8jjujj88CCCFCCFFCCCCFFF))rr2r22rr)r))))l))l))))llClCC=K=K=====KKcKcK<Y<<YY<Y<<k<<YYeKeeeeSS$SS$$$KJ>J>>XX>>J>J>>XM##$#$$$###$###l>>::>:::AAA:A:Y000ccc0cc?????_u__u_u_uuu)))w___..CC.CC.C.CC))!)!!@@@d@@@dd?1??k????1??1??,,222,,jj,j,jj.yyyyy<<ssss<s<s||]]h]]]h]hhhhj11m111111mmm1mmBLLAALAAALLAA|AYYyYyyyyqyyYyyy1uuuKKK?K?KK?K?whhhChhhhwhww##1;1;;;;M;;MMM;MzyyzyzyyyyyzzNz`a````aa``B```a~lll~lllNlNNNlNkk00kkcckccccckqpqqqqqqqpppUpU1@111##1#11###1!|!!;!!!|!!!!||;5;5;5;;;555,5,R3R33h3h3hh3h3hii>>>iiRRRiiii>sttsss7sss77<77353533555?55337okookooouu`uu`u^SS^^SSS^^^^WW^n,,nn888nn888n8...vv.vvqqqDDqq??[?[[####[[[[?))DDD))SSSS3S33;TTiiziiTiTTiiiUU~U~U~~~U~~~~~fLLLssMMssLLsLLOllOOnnOnOOllFltt~~-~~~~-~---6llqqll=l===ff==<OOttOOOtt(ttt(AA!!!A!!!A!AAcAN0N0NNPPJPPJPPJ'D''DD'D'''))H)FYFYFFZZFFFZZZPRyRRyRyRyyyyRRRmm***mm****x*x*``Q``Q``333~~3~11#1#11`1`11##kbb<<<<bb<<!<!<!MbbMbMbb444  4 >X>>00t00tt0t00w@@wwppwppwpwppaaDDaaDDDDSSIISh;h;h;hh;h;;;j;gMMPPMMggMgMgMg```a`aaaa``YY``$F$$V$V$$FFvFF$)*))**))*)***A*_?_____?__P___?iAAAiiAAiAAwww4~xxxx~xxzxxzzz$rsssrssmmm''m'mhhwhh>>?>>hhhwwm77m77''j''''jjvvvvv2v22++>+++++rr++=++=+===+PZPZPZPZPPPP:PPwWWWWwWwWwWWuuu--W-W-WW---NN-NciiDDDeDeeeeeDe.A...4...A.AA.Am&&&&EE&&m&&&EEx===M==MMXXXXMXvsvvv'v'v'v''v'~tt~ttt~~t~~~OO'_'__'''!!#!!##7_7_77<7<<y<y<<[J[[[JJJ$J$$r$r^^G^^^^GG^^G^^Aa]a]]a]aaGaGGaG3gggPgggPPPxx3x^!^!!^^^FFFFcFFIzIIrrr99r9r9r9PDDrDrDrrDDPPDD]OOxxxxOxxx[[[[ppSppnnn66nnpppW$W$W$$$s$sssswBBB[[QQQQWQQQQQ]w]ww]]ww]]w]w]NN99PPP?PP9PPP?==a=aaaaaRaaRRRPP,PPP,P,,,PPhh*9*99zzbzbzbzz9u:::!!ww!!w!wwwrkkkk_k_k_k_kk_<<aa~aaa<<11<1<DkkDkkDkDD*D***,,,,E,E,,,))),,oooooGGFFFGGFFG^~~3~~3~~~33o33NNRRNRRDDDDDHHD<?<??<<<<<<$$$wPPXPXXYY!YYXXXY~~~33~3333KKLKL;BBoBBBBBBoBooB2CCC`C`C`C``ZZ2QQ>>`>>QQ>>`>>`+ddd++dd((dd(dd>kkFkk>>k>kkFkkccc0cc^^6^6^^^c@@___@@__@@B@BBb??b??b???bb?b?~EE~~E~~~EE~~E~1___77_77_7__77`h``GGGDDGG``G`NNNNNiiFiFiFiFim99mmrrrrhhrrrmLDDD3DDDD3D3D3D((wwbww((((hhbhjiijijjjjjvjjvv$WWWWWss ss  s >>>;;>>99999+++||~|||~~oooo2oo.99..''.'..9..9d4d4d4d444ddzzdXvv44vvXvvvXXvXjjqqjj]]j]j]jjjUUUQUUUKUUUUQQQ----ddaadaaa55aX.XX.XXXX~X~XX.vTvvv&vvv&v&vvvuu((uuuuooGGoou6eeDD8D8DDeeeeeRR$R$RR;R;RR$$Ad|d||ddllldll+liigggggEEEEEEgEK7KK7K7K7K77..7xxxTxxyxyyQyyxy5g55ZZZ5ZZZwZZwN..N.NNNDNNDDDN( (((  U  U UU PAAggAgAgg<<<<<42424424242222pPP[[[RRRxxRxRxRkHkHH)H)))H))H)ZZFFFqqhqhqqhqheVeVVVeVVGVVVee:(:((((MMMMGMMGWJJ3JJ3J33'''W'L~~L~L~L~~~LL~LpprprrprppcppccqqBBqqBB7B7BBqq&&&v&&&v&vvv&vvT*T*T*TT**T****D^^~^~~~q~~q~qqVjVjjVjjj^^^8^^Y_YYY]YYY]]z]]Y++vv+vvv+vvSS Se'e''e''))')')'FFoFoo!!(!!(!(!SSSVSVVSSOSOOOO&W&WWWWwwWW&WW&((((_((((k(kkYYEE=EE==)==E=EE!166FFoFoFoo11oodEddEENEEEENNND5xxvvxvvxvxvvEE@@_@@_@@_@@@_@@&   == ====  ==NII3IIINNNINIII'')''''*'*'''))1z1111h1h11h11z;d;d;;;:;;d;d;d"
d = mm
print(mm)
print("flag{%s-%s-%s-%s-%s}" % (d[0:8],d[8:12],d[12:16],d[16:20],d[20:32]))