#include <stdio.h>
#include <stdlib.h>
#include "kvdb.h"
int main(){
kvdb_t *db=malloc(sizeof(kvdb_t));
kvdb_open(db,DB_FILE);
/* srand seed: 822278647*/
char* mystr;
kvdb_put(db, "EEE", "rtdugokxzc");
kvdb_put(db, "KKK", "awfagdvrylpfoxbbwmnhpd");
kvdb_put(db, "DDD", "fgzu");
kvdb_put(db, "AAA", "oqujhylducqrltjadzdyktifml");
printf("%s\n", (mystr = kvdb_get(db, "KKK")) == NULL ? "(null)" : mystr);
kvdb_put(db, "CCC", "rtdugokxzc");
kvdb_put(db, "LLL", "rignumizxxg");
kvdb_put(db, "DDD", "zeozudayazmgngbgmtljansliw");
kvdb_put(db, "KKK", "nqmzoafyhgngehbvvbslhgopuq");
printf("%s\n", (mystr = kvdb_get(db, "KKK")) == NULL ? "(null)" : mystr);
kvdb_put(db, "BBB", "jexetquuysbnlptgqojcxzl");
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
kvdb_put(db, "DDD", "tvilji");
kvdb_put(db, "GGG", "ytew");
kvdb_put(db, "BBB", "rcolwulvmlfwhmmhxdmhsiqvqsyn");
kvdb_put(db, "NNN", "nomqfsqmvqzbkqmlvjrtgqjxor");
kvdb_put(db, "GGG", "vzgmxrfffjlpudukoymzn");
printf("%s\n", (mystr = kvdb_get(db, "DDD")) == NULL ? "(null)" : mystr);
kvdb_put(db, "BBB", "jzbyeeqzlje");
kvdb_put(db, "LLL", "jxuczdrlrilhdorzbgtksbtpeitj");
kvdb_put(db, "DDD", "muldpeygca");
kvdb_put(db, "AAA", "ipljuasyzp");
printf("%s\n", (mystr = kvdb_get(db, "EEE")) == NULL ? "(null)" : mystr);
kvdb_put(db, "III", "lcloqvkyvhehjlh");
kvdb_put(db, "EEE", "rdlrwcktpgkflyz");
kvdb_put(db, "FFF", "xsunlyoejfbpzvuvrmyx");
kvdb_put(db, "AAA", "atssyqoirn");
kvdb_put(db, "AAA", "oukm");
kvdb_put(db, "AAA", "u");
kvdb_put(db, "NNN", "tevwcnxbddcasgfwqzbufhnbqz");
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
kvdb_put(db, "DDD", "pcfejreceeqwimzy");
kvdb_put(db, "EEE", "atssyqoirn");
printf("%s\n", (mystr = kvdb_get(db, "III")) == NULL ? "(null)" : mystr);
kvdb_put(db, "DDD", "yrzlkxmz");
kvdb_put(db, "HHH", "ojboamrtavbxb");
kvdb_put(db, "NNN", "fdspp");
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
kvdb_put(db, "PPP", "zqjrkjnakfhufixprtli");
printf("%s\n", (mystr = kvdb_get(db, "DDD")) == NULL ? "(null)" : mystr);
kvdb_put(db, "HHH", "kvivrbbfegnuoayjfwkzdu");
kvdb_put(db, "III", "rqpoi");
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "HHH")) == NULL ? "(null)" : mystr);
kvdb_put(db, "NNN", "bxftccrylot");
kvdb_put(db, "JJJ", "rcolwulvmlfwhmmhxdmhsiqvqsyn");
kvdb_put(db, "AAA", "akkaqokjeuatqkh");
printf("%s\n", (mystr = kvdb_get(db, "EEE")) == NULL ? "(null)" : mystr);
kvdb_put(db, "CCC", "yrzlkxmz");
kvdb_put(db, "CCC", "htdejgjbnr");
kvdb_put(db, "FFF", "kkbpedivqbeqhgvfqoqyddfnu");
kvdb_put(db, "PPP", "qx");
kvdb_put(db, "FFF", "uzxoithljuuhzhajsubresnrtwku");
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
kvdb_put(db, "PPP", "krkqfak");
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
kvdb_put(db, "NNN", "jznnmmndclyirwvfeea");
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "FFF")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
kvdb_put(db, "DDD", "ghbyqpydxzmfmutreqfxgchjtevftx");
kvdb_put(db, "FFF", "zqjrkjnakfhufixprtli");
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
kvdb_put(db, "OOO", "odqziabwmnhenfhkechepwrtsows");
kvdb_put(db, "NNN", "ovkagnigjxfvnd");
kvdb_put(db, "KKK", "izirwdlysiutbqkjwflriluqw");
kvdb_put(db, "JJJ", "vjbqnkqatjqktr");
kvdb_put(db, "GGG", "klnoypjgitjurlxr");
printf("%s\n", (mystr = kvdb_get(db, "DDD")) == NULL ? "(null)" : mystr);
kvdb_put(db, "GGG", "towmzqumzzcqydzrptvii");
printf("%s\n", (mystr = kvdb_get(db, "III")) == NULL ? "(null)" : mystr);
kvdb_put(db, "CCC", "dtwpudfzrxyiyajgkrpfgcl");
kvdb_put(db, "JJJ", "vprh");
kvdb_put(db, "AAA", "fic");
kvdb_put(db, "OOO", "irvldvqevzdaenmwgd");
kvdb_put(db, "LLL", "oukm");
printf("%s\n", (mystr = kvdb_get(db, "GGG")) == NULL ? "(null)" : mystr);
kvdb_put(db, "GGG", "htdejgjbnr");
kvdb_put(db, "CCC", "hgeupmato");
kvdb_put(db, "LLL", "xobdbudldimxblgxhberjvgfcry");
printf("%s\n", (mystr = kvdb_get(db, "BBB")) == NULL ? "(null)" : mystr);
kvdb_put(db, "EEE", "gqxbkblesntl");
kvdb_put(db, "GGG", "cieobravxxmeheotzfswuqvri");
kvdb_put(db, "PPP", "ouzheuyhgrfkd");
kvdb_put(db, "PPP", "lsjltvgyfwjuzjr");
kvdb_put(db, "NNN", "aft");
kvdb_put(db, "JJJ", "yvsrrxvefgxtcmbjeg");
kvdb_put(db, "PPP", "auwr");
kvdb_put(db, "OOO", "ztwcswqwtpvnqxwtt");
kvdb_put(db, "EEE", "jdjfgmrkvlgjgpihygnqdq");
kvdb_put(db, "NNN", "dkmrqqvwwgpuekpcgg");
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
kvdb_put(db, "AAA", "qx");
kvdb_put(db, "DDD", "qjtxgbuzzuvimqs");
kvdb_put(db, "GGG", "jexetquuysbnlptgqojcxzl");
kvdb_put(db, "EEE", "uotwehtasqpdxlkj");
printf("%s\n", (mystr = kvdb_get(db, "JJJ")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "EEE")) == NULL ? "(null)" : mystr);
kvdb_put(db, "HHH", "txduoqzbhjqnlzprjpfetzfeitok");
kvdb_put(db, "PPP", "laohypnmohrtocgdyl");
kvdb_put(db, "JJJ", "rqpoi");
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "HHH")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "CCC")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "JJJ")) == NULL ? "(null)" : mystr);
kvdb_put(db, "PPP", "fvpsnz");
kvdb_put(db, "NNN", "ufoajrdjntjagadxiu");
kvdb_put(db, "LLL", "yrxzgzjnnleqibnufwavqzbiwva");
kvdb_put(db, "HHH", "oqujhylducqrltjadzdyktifml");
kvdb_put(db, "LLL", "qjivmikitme");
printf("%s\n", (mystr = kvdb_get(db, "PPP")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "FFF")) == NULL ? "(null)" : mystr);
kvdb_put(db, "JJJ", "zeozudayazmgngbgmtljansliw");
printf("%s\n", (mystr = kvdb_get(db, "NNN")) == NULL ? "(null)" : mystr);
kvdb_put(db, "KKK", "rfyuibyktfsrvtspyacasec");
kvdb_put(db, "MMM", "kc");
kvdb_put(db, "GGG", "vraaiqdmaeyahx");
kvdb_put(db, "BBB", "ghpkxqhoiyztyoldnphnecjcfhao");
kvdb_put(db, "III", "jfmaxwqew");
printf("%s\n", (mystr = kvdb_get(db, "DDD")) == NULL ? "(null)" : mystr);
kvdb_put(db, "FFF", "xsunlyoejfbpzvuvrmyx");
kvdb_put(db, "OOO", "zhrdpumvaimmkiafzsvpcizb");
kvdb_put(db, "OOO", "tvxggwffebgwmxgxnhhuqezy");
printf("%s\n", (mystr = kvdb_get(db, "HHH")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "GGG")) == NULL ? "(null)" : mystr);
printf("%s\n", (mystr = kvdb_get(db, "LLL")) == NULL ? "(null)" : mystr);
kvdb_put(db, "JJJ", "qjtxgbuzzuvimqs");
printf("%s\n", (mystr = kvdb_get(db, "OOO")) == NULL ? "(null)" : mystr);
kvdb_close(db);
return 0;
}