
#define SETLENGTH 26//the set length
__constant unsigned char set[52] = { 
  
'a', 'b', 'c', 'd', 'e', 'f', 'g',
'h', 'i', 'j', 'k', 'l', 'm', 'n',
'o', 'p', 'q', 'r', 's', 't', 'u',
'v', 'w', 'x', 'y', 'z' ,

'A', 'B', 'C', 'D', 'E', 'F', 'G',
'H', 'I', 'J', 'K', 'L', 'M', 'N',
'O', 'P', 'Q', 'R', 'S', 'T', 'U',
'V', 'W', 'X', 'Y', 'Z'};

__kernel void md5(__global char* str_buf, int len, __global char* hash) {
  int id = get_global_id(0);
  unsigned int h0, h1, h2, h3, a, b, c, d;
  __global char *str = str_buf + id * 16;
  __private char buf[12];
  __private unsigned int *w = buf;
  int res = 0;
  __global int *h = hash;
  int ll = len*8;
  int w0, w1, w2;
  int hh0 = h[0] - 0x67452301;
  int hh1 = h[1] - 0xEFCDAB89;
  int hh2 = h[2] - 0x98BADCFE;
  int hh3 = h[3] - 0x10325476;

  w[0] = w[1] = w[2] = 0;
  for (int i = 0; i<len; ++i) {
    buf[i] = str[i];
  }
  buf[len] = 0x80;
  int ww = w[1];

  for (int t1=0; t1<SETLENGTH; ++t1) {
    buf[len-2] = set[t1];
    for (int t2=0; t2<SETLENGTH; ++t2) {
      buf[len-1] = set[t2];
      w0 = w[0];
      w1 = w[1];
      w2 = w[2];

      a = 0x67452301;
      b = 0xEFCDAB89;
      c = 0x98BADCFE;
      d = 0x10325476;
      a = rotate(a+((b&c)|((~b)&d))+3614090360u+w0,7u)+b;
      d = rotate(d+((a&b)|((~a)&c))+3905402710u+w1,12u)+a;
      c = rotate(c+((d&a)|((~d)&b))+606105819u+w2,17u)+d;
      b = rotate(b+((c&d)|((~c)&a))+3250441966u,22u)+c;
      a = rotate(a+((b&c)|((~b)&d))+4118548399u,7u)+b;
      d = rotate(d+((a&b)|((~a)&c))+1200080426u,12u)+a;
      c = rotate(c+((d&a)|((~d)&b))+2821735955u,17u)+d;
      b = rotate(b+((c&d)|((~c)&a))+4249261313u,22u)+c;
      a = rotate(a+((b&c)|((~b)&d))+1770035416u,7u)+b;
      d = rotate(d+((a&b)|((~a)&c))+2336552879u,12u)+a;
      c = rotate(c+((d&a)|((~d)&b))+4294925233u,17u)+d;
      b = rotate(b+((c&d)|((~c)&a))+2304563134u,22u)+c;
      a = rotate(a+((b&c)|((~b)&d))+1804603682u,7u)+b;
      d = rotate(d+((a&b)|((~a)&c))+4254626195u,12u)+a;
      c = rotate(c+((d&a)|((~d)&b))+2792965006u+ll,17u)+d;
      b = rotate(b+((c&d)|((~c)&a))+1236535329u,22u)+c;

      a = rotate(a+((d&b)|((~d)&c))+4129170786u+w1,5u)+b;
      d = rotate(d+((c&a)|((~c)&b))+3225465664u,9u)+a;
      c = rotate(c+((b&d)|((~b)&a))+643717713u,14u)+d;
      b = rotate(b+((a&c)|((~a)&d))+3921069994u+w0,20u)+c;
      a = rotate(a+((d&b)|((~d)&c))+3593408605u,5u)+b;
      d = rotate(d+((c&a)|((~c)&b))+38016083u,9u)+a;
      c = rotate(c+((b&d)|((~b)&a))+3634488961u,14u)+d;
      b = rotate(b+((a&c)|((~a)&d))+3889429448u,20u)+c;
      a = rotate(a+((d&b)|((~d)&c))+568446438u,5u)+b;
      d = rotate(d+((c&a)|((~c)&b))+3275163606u+ll,9u)+a;
      c = rotate(c+((b&d)|((~b)&a))+4107603335u,14u)+d;
      b = rotate(b+((a&c)|((~a)&d))+1163531501u,20u)+c;
      a = rotate(a+((d&b)|((~d)&c))+2850285829u,5u)+b;
      d = rotate(d+((c&a)|((~c)&b))+4243563512u+w2,9u)+a;
      c = rotate(c+((b&d)|((~b)&a))+1735328473u,14u)+d;
      b = rotate(b+((a&c)|((~a)&d))+2368359562u,20u)+c;

      a = rotate(a+(b^c^d)+4294588738u,4u)+b;
      d = rotate(d+(a^b^c)+2272392833u,11u)+a;
      c = rotate(c+(d^a^b)+1839030562u,16u)+d;
      b = rotate(b+(c^d^a)+4259657740u+ll,23u)+c;
      a = rotate(a+(b^c^d)+2763975236u+w1,4u)+b;
      d = rotate(d+(a^b^c)+1272893353u,11u)+a;
      c = rotate(c+(d^a^b)+4139469664u,16u)+d;
      b = rotate(b+(c^d^a)+3200236656u,23u)+c;
      a = rotate(a+(b^c^d)+681279174u,4u)+b;
      d = rotate(d+(a^b^c)+3936430074u+w0,11u)+a;
      c = rotate(c+(d^a^b)+3572445317u,16u)+d;
      b = rotate(b+(c^d^a)+76029189u,23u)+c;
      a = rotate(a+(b^c^d)+3654602809u,4u)+b;
      d = rotate(d+(a^b^c)+3873151461u,11u)+a;
      c = rotate(c+(d^a^b)+530742520u,16u)+d;
      b = rotate(b+(c^d^a)+3299628645u+w2,23u)+c;

      a = rotate(a+(c^(b|(~d)))+4096336452u+w0,6u)+b;
      d = rotate(d+(b^(a|(~c)))+1126891415u,10u)+a;
      c = rotate(c+(a^(d|(~b)))+2878612391u+ll,15u)+d;
      b = rotate(b+(d^(c|(~a)))+4237533241u,21u)+c;
      a = rotate(a+(c^(b|(~d)))+1700485571u,6u)+b;
      d = rotate(d+(b^(a|(~c)))+2399980690u,10u)+a;
      c = rotate(c+(a^(d|(~b)))+4293915773u,15u)+d;
      b = rotate(b+(d^(c|(~a)))+2240044497u+w1,21u)+c;
      a = rotate(a+(c^(b|(~d)))+1873313359u,6u)+b;
      d = rotate(d+(b^(a|(~c)))+4264355552u,10u)+a;
      c = rotate(c+(a^(d|(~b)))+2734768916u,15u)+d;
      b = rotate(b+(d^(c|(~a)))+1309151649u,21u)+c;
      a = rotate(a+(c^(b|(~d)))+4149444226u,6u)+b;
      d = rotate(d+(b^(a|(~c)))+3174756917u,10u)+a;
      c = rotate(c+(a^(d|(~b)))+718787259u+w2,15u)+d;
      b = rotate(b+(d^(c|(~a)))+3951481745u,21u)+c;


      if (!((a ^ hh0) | (b ^ hh1) | (c ^ hh2) | (d ^ hh3))) {
        res = (buf[len-2]<<8)|(buf[len-1]);
      }
    }
  }
  __global unsigned int *g = str;
  g[0] = res;
}
