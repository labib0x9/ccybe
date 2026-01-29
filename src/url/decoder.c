#include"decoder.h"


/*  
    URL_DECODING....%%....

    Cheat-Sheet: https://www.cheat-sheets.org/sites/html.su/urlencoding.html

    Labib%3C/script%3E%22+%27&=
    Labib</script>"+'&=


    http://example.com/static/files/my file (v1)/report#1.txt?q=hello world+plus&token=a/b=c&path=../etc/passwd

    http://example.com/static/files/my%20file%20(v1)/report%231.txt?q=hello%20world%2Bplus&token=a%2Fb%3Dc&path=..%2Fetc%2Fpasswd


*/

// %2x
char get_2_char(char x) {
    switch(x) {
        case '0': return ' ';
        case '2': return '\"';
        case '3': return '#';
        case '5': return '%';
        case '7': return '\'';
        case 'B': return '+';
        case 'F': return '/';
        // default: return '\0';
    }
    return '\0';
}

// %3x
char get_3_char(char x) {
    switch(x) {
        case 'A': return ':';
        case 'B': return ';';
        case 'C': return '<';
        case 'D': return '=';
        case 'E': return '>';
        case 'F': return '?';
        // default: return '\0';
    }
    return '\0';
}


int decode_url(char* url, char* dec_url) {
    int n = strlen(url);
    int len = 0;

    for (int i = 0; i < n;) {
        if (url[i] == '%' && i < n - 3) {
            switch(url[i + 1]) {
                case '2': {
                    char x = get_2_char(url[i + 2]);
                    if (x == '\0') {
                        printf("invalid - 2: url[%d]=%c\n", i + 2, url[i + 2]);
                        // goto BREAK;
                        return -1;
                    }
                    dec_url[len++] = x;
                    break;
                }
                case '3': {
                    char x = get_3_char(url[i + 2]);
                    if (x == '\0') {
                        printf("invalid - 3: url[%d]=%c\n", i + 2, url[i + 2]);
                        // goto BREAK;
                        return -1;
                    }
                    dec_url[len++] = x;
                    break;
                }
                default: {
                    // perror("No matched encoded");
                    printf("enc_url[%d]=%c\n", i + 1, url[i + 1]);
                    // goto BREAK;
                    return -1;
                }
            }
            i += 3;
        } else {
            // printf("enc_url[%d]=%c\n", i, enc_url[i]);
            dec_url[len++] = url[i++];
        }

        // continue;

        // BREAK:
        //     break;
    }
    dec_url[len] = '\0';
    return 0;
}