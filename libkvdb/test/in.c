#include "kvdb.h"
#include <stdio.h>
#include <stdlib.h>
int main(){
kvdb_t *db=malloc(sizeof(kvdb_t));
kvdb_open(db,"./auto.db");
/* srand seed: 1024*/
char* mystr;
kvdb_put(db, "KKK", "fnk");
printf("%s\n", (mystr = kvdb_get(db, "KKK")) == NULL ? "(null)" : mystr);
kvdb_put(db, "LLL", "ftimwqkbspscncosg");
kvdb_put(db, "NNN", "fuxmkyvwwzztshhwzxfhhshuheuxe");
kvdb_put(db, "MMM", "mdnjkagctxvrjvvb");
kvdb_put(db, "PPP", "fqtcgskhxs");
kvdb_put(db, "NNN", "iohtkvnsegwru");
kvdb_put(db, "LLL", "iofdrcyfyhhjlcywmcrm");
kvdb_put(db, "KKK", "hzfwfqrirwhkkdlpzkhya");
printf("%s\n", (mystr = kvdb_get(db, "KKK")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
kvdb_put(db, "NNN", "uqprvbi");
printf("%s\n", (mystr = kvdb_get(db, "MMM")) == NULL ? "(null)" : mystr);
kvdb_put(db, "PPP", "wduatfmuywudsb");
kvdb_put(db, "FFF", "gcbqjclkytxiflw");
kvdb_put(db, "LLL", "qwmqxyloku");
kvdb_put(db, "KKK", "kfdtifyunxizqupno");
kvdb_put(db, "DDD", "ytqpyrnpaegcpkouuqlxp");
kvdb_put(db, "AAA", "axzmijohrcwzvcov");
kvdb_put(db, "III", "lcb");
printf("%s\n", (mystr = kvdb_get(db, "DDD")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "III")) == NULL ? "(null)" : mystr);
kvdb_put(db, "LLL", "spxmrvkcrwjevdhsotrrb");
kvdb_put(db, "EEE", "fpjbdglqcgwon");
printf("%s\n", (mystr = kvdb_get(db, "EEE")) == NULL ? "(null)" : mystr);
kvdb_put(db, "MMM", "vwp");
printf("%s\n", (mystr = kvdb_get(db, "MMM")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "KKK")) == NULL ? "(null)" : mystr);
kvdb_put(db, "HHH", "vlqhggrmit");
printf("%s\n", (mystr = kvdb_get(db, "AAA")) == NULL ? "(null)" : mystr);
kvdb_put(db, "CCC", "jyhgyalsrmjnqbcndezchtt");
kvdb_put(db, "CCC", "whwhlbfmuymmca");
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
kvdb_put(db, "CCC", "ljmangbzxduyvdxwwh");
kvdb_put(db, "FFF", "vkykneubkqcuzqhwyrkkcqzqxcshcbi");
kvdb_put(db, "OOO", "izknhehmhxnb");
kvdb_put(db, "CCC", "xvdmmsqfnvikxwchpkldspl");
printf("%s\n", (mystr = kvdb_get(db, "CCC")) == NULL ? "(null)" : mystr);
kvdb_put(db, "LLL", "bxirwmleomooyxcyli");
kvdb_put(db, "CCC", "hsvwororksznwcavt");
kvdb_put(db, "CCC", "bsianojmxigibpyvsdlx");
kvdb_put(db, "NNN", "pggenes");
kvdb_put(db, "PPP", "hzfwfqrirwhkkdlpzkhya");
kvdb_put(db, "FFF", "fqtcgskhxs");
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
kvdb_put(db, "OOO", "susmdchxkvckbhowh");
kvdb_put(db, "NNN", "rzgsbhggtfcjuachrdnxhdivzelk");
kvdb_put(db, "PPP", "fuxmkyvwwzztshhwzxfhhshuheuxe");
kvdb_put(db, "AAA", "zbveyhbcd");
printf("%s\n", (mystr = kvdb_get(db, "FFF")) == NULL ? "(null)" : mystr);
kvdb_put(db, "JJJ", "yajzooptqbhqoinbfhnthq");
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
kvdb_put(db, "FFF", "gxomsjnvuj");
kvdb_put(db, "OOO", "hgkkupjccrnqipymcphbup");
kvdb_put(db, "FFF", "pgnhjvhs");
printf("%s\n", (mystr = kvdb_get(db, "OOO")) == NULL ? "(null)" : mystr);
kvdb_put(db, "CCC", "dpuhvdun");
kvdb_put(db, "HHH", "atkaytcwxalmo");
kvdb_put(db, "CCC", "vnvtjpt");
kvdb_put(db, "NNN", "afrdbten");
kvdb_put(db, "EEE", "cv");
kvdb_put(db, "NNN", "ysi");
kvdb_put(db, "III", "ojuwtthqefrvmqrczxbxueblwgulq");
printf("%s\n", (mystr = kvdb_get(db, "CCC")) == NULL ? "(null)" : mystr);
kvdb_put(db, "AAA", "rnawcbljbxnl");
printf("%s\n", (mystr = kvdb_get(db, "FFF")) == NULL ? "(null)" : mystr);
kvdb_put(db, "BBB", "eovgpw");
kvdb_put(db, "DDD", "il");
kvdb_put(db, "LLL", "aariacblurahbaadnhvudmaapt");
kvdb_put(db, "NNN", "xpvwhbfqiqowzjyklfpt");
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
kvdb_put(db, "FFF", "uccodsuakfiuscb");
kvdb_put(db, "DDD", "bmvcundb");
printf("%s\n", (mystr = kvdb_get(db, "AAA")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "BBB")) == NULL ? "(null)" : mystr);
kvdb_put(db, "FFF", "zrghjhkymhvclzhditwgykbvtpy");
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
kvdb_put(db, "HHH", "wvdcpadlnbvegfxdbphiwds");
kvdb_put(db, "LLL", "slngugavunmcgtu");
kvdb_put(db, "MMM", "jkonb");
kvdb_put(db, "HHH", "gxomsjnvuj");
kvdb_put(db, "LLL", "hubamlklkjsrrnznudjujihvet");
kvdb_put(db, "KKK", "yvzdsbvlunelltui");
kvdb_put(db, "GGG", "ssrsksrvculbvqjtwujpzurtm");
kvdb_put(db, "III", "ksgaereozfuwuy");
printf("%s\n", (mystr = kvdb_get(db, "III")) == NULL ? "(null)" : mystr);
kvdb_put(db, "HHH", "kfdtifyunxizqupno");
printf("%s\n", (mystr = kvdb_get(db, "FFF")) == NULL ? "(null)" : mystr);
kvdb_put(db, "KKK", "pymyyqx");
kvdb_put(db, "FFF", "losgtxytajjden");
kvdb_put(db, "BBB", "xmeplwosghqcnmoernvh");
printf("%s\n", (mystr = kvdb_get(db, "JJJ")) == NULL ? "(null)" : mystr);
kvdb_put(db, "AAA", "gfevvlxbtj");
printf("%s\n", (mystr = kvdb_get(db, "EEE")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "KKK")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "DDD")) == NULL ? "(null)" : mystr);
kvdb_put(db, "GGG", "wikessn");
kvdb_put(db, "III", "wduatfmuywudsb");
kvdb_put(db, "AAA", "odnsqaoki");
kvdb_put(db, "FFF", "jnir");
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
kvdb_put(db, "HHH", "twfksojurtwhpxjforrvpyv");
kvdb_put(db, "EEE", "dcqwxrxrjzxvtgvxzbnyhhwbfkuhbf");
kvdb_put(db, "III", "sylrctnwcjeopziedyb");
printf("%s\n", (mystr = kvdb_get(db, "FFF")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "MMM")) == NULL ? "(null)" : mystr);
kvdb_put(db, "OOO", "kizduoeoxceuplryssbzspc");
kvdb_put(db, "GGG", "jnir");
kvdb_put(db, "PPP", "xzkslctbuwbtkbxrkxxficztscvyf");
kvdb_put(db, "MMM", "fblgdqxirsmfkrgefahlloyoxfwdr");
kvdb_put(db, "CCC", "jrksopnk");
printf("%s\n", (mystr = kvdb_get(db, "OOO")) == NULL ? "(null)" : mystr);
kvdb_put(db, "CCC", "tidmicfgxbzolj");
kvdb_put(db, "PPP", "fqtcgskhxs");
kvdb_put(db, "MMM", "hgkkupjccrnqipymcphbup");
kvdb_put(db, "OOO", "xmeplwosghqcnmoernvh");
kvdb_put(db, "HHH", "lbwnxaotqdlaboffprkogz");
kvdb_put(db, "GGG", "rzgsbhggtfcjuachrdnxhdivzelk");
kvdb_put(db, "NNN", "dcqwxrxrjzxvtgvxzbnyhhwbfkuhbf");
printf("%s\n", (mystr = kvdb_get(db, "GGG")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "AAA")) == NULL ? "(null)" : mystr);
kvdb_put(db, "GGG", "wvdcpadlnbvegfxdbphiwds");
printf("%s\n", (mystr = kvdb_get(db, "BBB")) == NULL ? "(null)" : mystr);
kvdb_put(db, "HHH", "spxmrvkcrwjevdhsotrrb");
kvdb_put(db, "BBB", "aswurxodrkjadrscpbawode");
kvdb_close(db);
return 0;
}