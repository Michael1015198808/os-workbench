#include <stdio.h>
#include <stdlib.h>
#include "kvdb.h"
int main(){
kvdb_t *db=malloc(sizeof(kvdb_t));
kvdb_open(db,DB_FILE);
/* srand seed: 1006987514*/
char* mystr;
kvdb_put(db, "OOO", "tgt");
kvdb_put(db, "III", "lezlnvfcaukypkvtljoigjrucbcurf");
kvdb_put(db, "NNN", "zdcsvsh");
printf("%s\n", (mystr = kvdb_get(db, "III")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "OOO")) == NULL ? "(null)" : mystr);
kvdb_put(db, "FFF", "turcyeezvrgdhybsdvnsmjzjap");
kvdb_put(db, "CCC", "jscfmdpsaosxkjpzazm");
kvdb_put(db, "III", "tycexqwxvjwdzzoxbgvg");
kvdb_put(db, "OOO", "biatixtmwbuvfsicm");
kvdb_put(db, "PPP", "xrmccqkijeltgfxjhxsoxmgqeksgu");
printf("%s\n", (mystr = kvdb_get(db, "OOO")) == NULL ? "(null)" : mystr);
kvdb_put(db, "OOO", "oivylnwhpkevwnejzycazzokropg");
printf("%s\n", (mystr = kvdb_get(db, "III")) == NULL ? "(null)" : mystr);
kvdb_put(db, "AAA", "usiswmbaw");
kvdb_put(db, "CCC", "uuugyldilitwkzwgynecoccmbpb");
printf("%s\n", (mystr = kvdb_get(db, "OOO")) == NULL ? "(null)" : mystr);
kvdb_put(db, "KKK", "z");
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
kvdb_put(db, "NNN", "lqo");
kvdb_put(db, "KKK", "zdckbhyzststwpy");
printf("%s\n", (mystr = kvdb_get(db, "CCC")) == NULL ? "(null)" : mystr);
kvdb_put(db, "NNN", "qvsahkbwvbh");
printf("%s\n", (mystr = kvdb_get(db, "KKK")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
kvdb_put(db, "LLL", "cqlcipeoigghikuy");
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
kvdb_put(db, "OOO", "rnustnakpwhvdrddtwlsyxcgjb");
kvdb_put(db, "FFF", "ty");
kvdb_put(db, "CCC", "a");
printf("%s\n", (mystr = kvdb_get(db, "OOO")) == NULL ? "(null)" : mystr);
kvdb_put(db, "KKK", "htycjh");
kvdb_put(db, "III", "wgupvpujthfzkjzrfptrf");
printf("%s\n", (mystr = kvdb_get(db, "FFF")) == NULL ? "(null)" : mystr);
kvdb_put(db, "PPP", "z");
kvdb_put(db, "AAA", "edpmvuecwindzewk");
kvdb_put(db, "EEE", "usiswmbaw");
kvdb_put(db, "NNN", "dbnkbpdjmuwqwtcpluoxkmvngzr");
kvdb_put(db, "MMM", "zhjsatlumodp");
kvdb_put(db, "EEE", "fgewnjyddcmqs");
kvdb_put(db, "JJJ", "usiswmbaw");
printf("%s\n", (mystr = kvdb_get(db, "EEE")) == NULL ? "(null)" : mystr);
kvdb_put(db, "GGG", "olynlhn");
kvdb_put(db, "EEE", "blmdcrtfjsonhtsyaulpwcqwot");
kvdb_put(db, "AAA", "dhwqangretxwwgflmp");
kvdb_put(db, "III", "rdkpsmdfpixxlmlhmzf");
printf("%s\n", (mystr = kvdb_get(db, "MMM")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "JJJ")) == NULL ? "(null)" : mystr);
kvdb_put(db, "PPP", "umzsmqxlodnmaurehswdtsdzbvukgyo");
kvdb_put(db, "EEE", "vdhtzvrcpjgqarhbikptlee");
printf("%s\n", (mystr = kvdb_get(db, "III")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
kvdb_put(db, "BBB", "dxopyetiqavxefkgaasgzakihmxpbsz");
kvdb_put(db, "EEE", "tycexqwxvjwdzzoxbgvg");
kvdb_put(db, "GGG", "mpvahsmwxbixxzeetcimun");
kvdb_put(db, "HHH", "jvg");
kvdb_put(db, "GGG", "vdhtzvrcpjgqarhbikptlee");
kvdb_put(db, "AAA", "jjpcojusif");
printf("%s\n", (mystr = kvdb_get(db, "KKK")) == NULL ? "(null)" : mystr);
kvdb_put(db, "DDD", "xrmccqkijeltgfxjhxsoxmgqeksgu");
kvdb_put(db, "LLL", "saxqujo");
printf("%s\n", (mystr = kvdb_get(db, "GGG")) == NULL ? "(null)" : mystr);
kvdb_put(db, "EEE", "fgewnjyddcmqs");
printf("%s\n", (mystr = kvdb_get(db, "HHH")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
kvdb_put(db, "JJJ", "jscfmdpsaosxkjpzazm");
kvdb_put(db, "BBB", "yatidinoujlmfgpurlkoyylnfket");
kvdb_put(db, "PPP", "qelpmmqavnbbfd");
printf("%s\n", (mystr = kvdb_get(db, "DDD")) == NULL ? "(null)" : mystr);
kvdb_put(db, "MMM", "oywlncb");
kvdb_put(db, "HHH", "a");
kvdb_put(db, "BBB", "thwxfsmldojhqigeuysketcoib");
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
kvdb_put(db, "KKK", "gizdftbtuzzrbt");
kvdb_put(db, "III", "jmakafaxdueshxmchowoccjyvec");
kvdb_put(db, "FFF", "odiypcxrjvpfqzxttgjonwnnvpw");
printf("%s\n", (mystr = kvdb_get(db, "BBB")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "MMM")) == NULL ? "(null)" : mystr);
kvdb_put(db, "MMM", "tmebtzprgpzgvmabcienhdkk");
kvdb_put(db, "PPP", "atomu");
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "EEE")) == NULL ? "(null)" : mystr);
kvdb_put(db, "BBB", "z");
printf("%s\n", (mystr = kvdb_get(db, "HHH")) == NULL ? "(null)" : mystr);
kvdb_put(db, "BBB", "wgupvpujthfzkjzrfptrf");
printf("%s\n", (mystr = kvdb_get(db, "JJJ")) == NULL ? "(null)" : mystr);
kvdb_put(db, "DDD", "a");
kvdb_put(db, "OOO", "usiswmbaw");
kvdb_put(db, "LLL", "vfjpeqvbqwsbsbu");
kvdb_put(db, "HHH", "adudduqnjjfozqfzqwx");
kvdb_put(db, "GGG", "btgxdjijumvofvpjfhlezswy");
kvdb_put(db, "BBB", "twhcnztrirrtdjarr");
kvdb_put(db, "KKK", "wwibjflbkut");
printf("%s\n", (mystr = kvdb_get(db, "MMM")) == NULL ? "(null)" : mystr);
kvdb_put(db, "OOO", "wwibjflbkut");
kvdb_put(db, "AAA", "dzfftpk");
kvdb_put(db, "MMM", "vrpjupubbppgmxlphcjzudblyj");
kvdb_put(db, "CCC", "a");
kvdb_put(db, "NNN", "iybskmiosfzrgq");
printf("%s\n", (mystr = kvdb_get(db, "FFF")) == NULL ? "(null)" : mystr);
kvdb_put(db, "KKK", "vevkttkscsqamnbogydmqqc");
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
kvdb_put(db, "III", "ljhnmrrzbullqadokz");
kvdb_put(db, "JJJ", "blmdcrtfjsonhtsyaulpwcqwot");
kvdb_put(db, "HHH", "gxatldkquomcexuqtjnf");
kvdb_put(db, "GGG", "jolmkgfrbygaqa");
kvdb_put(db, "OOO", "geahfjgonvwksgcqg");
printf("%s\n", (mystr = kvdb_get(db, "GGG")) == NULL ? "(null)" : mystr);
kvdb_put(db, "NNN", "kowxorkcpxkxcjlc");
kvdb_put(db, "KKK", "a");
kvdb_put(db, "OOO", "lqo");
kvdb_put(db, "III", "wcjadvojxfcc");
kvdb_put(db, "DDD", "diycchekwmayhdprfdjoqyfuxgtun");
kvdb_put(db, "III", "jhapfpt");
kvdb_put(db, "KKK", "ca");
kvdb_put(db, "NNN", "dxopyetiqavxefkgaasgzakihmxpbsz");
printf("%s\n", (mystr = kvdb_get(db, "BBB")) == NULL ? "(null)" : mystr);
kvdb_put(db, "MMM", "atomu");
kvdb_put(db, "AAA", "wcjadvojxfcc");
printf("%s\n", (mystr = kvdb_get(db, "KKK")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "HHH")) == NULL ? "(null)" : mystr);
kvdb_put(db, "DDD", "cqlcipeoigghikuy");
kvdb_put(db, "LLL", "ezfuqbmofbfvhnduh");
printf("%s\n", (mystr = kvdb_get(db, "JJJ")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "OOO")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
kvdb_put(db, "OOO", "vifvjcnabqmfios");
kvdb_close(db);
return 0;
}