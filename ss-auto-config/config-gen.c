#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

char* random_pass(int length, int seed);
int random_port(int min, int max, int seed);

/*
 *
 * Required: qrencode - https://github.com/fukuchi/libqrencode
 *           ssserver - https://shadowsocks.org
 *           lnmp - http://lnmp.org/
 *
 * Bugs: Unfriendly config of program required arguments
 *       Something wrong when restart shadowsocks service
 *
 */

const int MAX_LINE_LENGTH = 1024;
const char *PATH_TEMPLATE = "Path-of-config-file-template";
const char *PATH_TARGET = "Path-for-generated-config-file";
const char *PATH_PORT_HTML = "Path-for-saving-port-file";
const char *PATH_PASSWORD_HTML = "Path-for-saving-password-file";
const char *PATH_QRCODE = "Path-for-saving-qrcode";
const char *SERVER_IP = "xxx.xxx.xxx.xxx";
const char *METHOD = "xxx-xxx-xxx";


int main()
{

    FILE *fp_template = fopen(PATH_TEMPLATE, "r");
    FILE *fp_target = fopen(PATH_TARGET, "w");
    FILE *fp_port = fopen(PATH_PORT_HTML, "w");
    FILE *fp_password = fopen(PATH_PASSWORD_HTML, "w");

    char template_content[MAX_LINE_LENGTH];
    int real_port = random_port(32768, 65535, time(0));
    char *real_pass = random_pass(10, time(0));


    fprintf(fp_port, "<i>%d</i>", real_port);
    fprintf(fp_password, "<i><font size='3'>%s</font></i>", real_pass);

    fclose(fp_port);
    fclose(fp_password);

    char part_config[1024];
    sprintf(part_config, "        \"%d\": \"%s\",\n", real_port, real_pass);

    while ((fgets(template_content, MAX_LINE_LENGTH, fp_template)) != NULL)
    {

        if(strcmp(template_content, "<mark>\n") == 0)
        {
            fprintf(fp_target, "%s", part_config);
        }else {
            fprintf(fp_target, "%s", template_content);
        }
    }

    fclose(fp_target);
    fclose(fp_template);

    //The detail of shadowsocks's QRcode: https://shadowsocks.org/en/config/quick-guide.html
    //generate the base64 code of config and create the QR code.
    char qrcode_text[1024];
    char base64_str[1024];
    sprintf(qrcode_text, "echo -n 'aes-256-cfb:%s@104.129.0.236:%d' | base64", real_pass, real_port);
    FILE *base64_encode = popen(qrcode_text, "r");
    fscanf(base64_encode, "%s", base64_str);
    pclose(base64_encode);

    //generate the QRcode picture and save into web root directory.
    char qrcode_gen[1024];
    sprintf(qrcode_gen, "qrencode -o %s -s 10 -m 1 -l H 'ss://%s'",PATH_QRCODE , base64_str);
    FILE *exec_qrencode = popen(qrcode_gen, "w");
    pclose(exec_qrencode);

    //restart ss server
    FILE *kill_ssserver = popen("killall ssserver", "w");
    pclose(kill_ssserver);

    FILE *exec = popen("ssserver -c /home/shadowsocks/shadowsocks.json --log-file /home/shadowsocks/ss-user.log -d start", "w");
    pclose(exec);

    return 0;
}

int random_port(int min, int max, int seed)
{
    int random;
    srand(time(0) + seed);
    random = rand() % (max - min + 1) + min;
    return random;
}


char* random_pass(int length, int seed)
{
    int type, i;
    char *randomStr = (char*)malloc(length * (sizeof(char) + 1));
    char currentChar, rangeMin, rangeMax;

    for(i = 0; i < length; i++)
    {
        srand(time(0) + (seed + 7) * (i + 10));
        type = rand() % 3;
        switch (type) {
            case 0 :
                rangeMin = 'A';
                rangeMax = 'Z';
                break;
            case 1:
                rangeMin = 'a';
                rangeMax = 'z';
                break;
            case 2:
                rangeMin = '0';
                rangeMax = '9';
                break;
            default:
                rangeMin = 'A';
                rangeMax = 'Z';
        }
        currentChar = rand() % (rangeMax - rangeMin + 1) + rangeMin;
        randomStr[i] = currentChar;
    }
    randomStr[i] = '\0';
    return randomStr;
}
