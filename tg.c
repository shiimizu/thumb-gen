#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sapi/embed/php_embed.h>
#include <koio.h>

void koio_load_assets(void);
void koio_unload_assets(void);

char *ltrim(char *);
char *rtrim(char *);
char *trim2(char *);
char *trim(char *);
char *trim_php_ends(char *);

int main(int argc, char *argv[])
{
    /* declare a file pointer */
    FILE    *src;
    char    *buffer;
    long    numbytes;
    const char* script = "tg.php";
     
    /* open an existing file for reading */
    koio_load_assets();
    src = ko_fopen("//tg.php", "r");
    // src = fopen(script, "r");    
     
    /* quit if the file does not exist */
    if(src == NULL) {
        fprintf(stderr, "An error occurred trying to open the file: `%s`\n", script);
        script = NULL;
        return 1;
    }
     
    /* Get the number of bytes */
    fseek(src, 0L, SEEK_END);
    numbytes = ftell(src);
     
    /* reset the file position indicator to 
    the beginning of the file */
    fseek(src, 0L, SEEK_SET);	
     
    /* grab sufficient memory for the 
    buffer to hold the text */
    buffer = (char*)calloc(numbytes, sizeof(char));	
     
    /* memory error */
    if(buffer == NULL) {
        fprintf(stderr, "A memory error occurred trying to open the file: `%s`\n", script);
        fclose(src);
        koio_unload_assets();
        src = NULL;
        script = NULL;
        return 1;
    }
     
    /* copy all the text into the buffer */
    fread(buffer, sizeof(char), numbytes, src);
    buffer[numbytes] = '\0';
    fclose(src);
    koio_unload_assets();
    src = NULL;
     
    /* confirm we have read the file by
    outputing it to the console */
    PHP_EMBED_START_BLOCK(argc, argv)
        buffer = trim_php_ends(buffer);
        zend_eval_string(buffer, NULL, (char *)script TSRMLS_CC);
        
        /* free the memory we used for the buffer */
        if(buffer) {
            free(buffer);
            buffer = NULL;
        }
    PHP_EMBED_END_BLOCK()

    return 0;

}

char *trim_php_ends(char *str) {
    if(!str) return str;
    str = trim(str);
    
    size_t len = strlen(str);
    if( len >= 5 &&
        str[0] == '<' &&
        str[1] == '?' &&
        str[2] == 'p' &&
        str[3] == 'h' &&
        str[4] == 'p') {
        memmove(str, str + 5, len - 5 + 1);
        // str += 5; // don't use for dynamic strings
    }
    
    len = strlen(str);
    char *end = str + len - 1;
    if(end && len >= 2) {
        if (end && *end == '>'){ *end = '\0' ; end--; } ;
        if (end && *end == '?'){ *end = '\0' ; end--; } ;
    }
    return str;
}

char *trim(char *string)
{
    if(!string) return string;
    
    char *ptr = NULL;
    
    // chomp away space at the start
    while (isspace(*string)) 
        memmove(string, string + 1, strlen(string));
        // string++; 
    
    // jump to the last char (-1 because '\0')
    ptr = string + strlen(string) - 1;
    
    // overwrite with end of string
    while (isspace(*ptr)){ *ptr = '\0' ; ptr--; } ;
    
    return string;  // return pointer to the modified start 
}

char *trim2(char *s)
{
    if(!s) return s;
    return rtrim(ltrim(s)); 
}

char *ltrim(char *s)
{
    if(!s) return s;
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    if(!s) return s;
    char* back = s + strlen(s);
    if(!back) return s;
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}