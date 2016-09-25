#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAMLEX 1000

typedef struct 
{
	char lexema[TAMLEX];
	char complex[20];
}Token;

Token t;
int is_scape(char c);
void consumir();
FILE *archivo_fuente;
FILE *archivo_salida;
int estado_error;
int linea=1;
int co=0;
char c;

void sig_lex(){

	c=0;
	int estado=0;
	int estado_aceptacion;
    estado_error=0;
	memset(t.lexema, 0,sizeof t.lexema);
	memset(t.complex, 0,sizeof t.complex);
	if((c=fgetc(archivo_fuente))!= EOF)
	{
		if(c==' ' || c=='\t')
		{
           fprintf(archivo_salida,"%c",c);
		}else if(c=='\n')
		{
			linea++;
			fprintf(archivo_salida,"%c",c);

		}
		else if(c=='\"') //Es un STRING
		{ 
			estado=1;
			estado_aceptacion=0;
			int i=0;
			t.lexema[i]=c;
			while(!estado_aceptacion)
			{
				switch(estado)
				{
					case(1):
						c=fgetc(archivo_fuente);
						if( c != '\\' && c!= '\"')
						{
							estado=2;
							t.lexema[++i]=c;
						}
						else if(c=='\\')
						{
							estado=3;
							t.lexema[++i]=c;
						}
						else if(c=='\"')
						{
							estado=5;
							t.lexema[++i]=c;
						}
						else if(c==EOF)
						{
							estado=-1;
						}	
						break;

					case(2):
						c=fgetc(archivo_fuente);
						if(c != '\\' && c!= '\"')
						{
							estado=2;
							t.lexema[++i]=c;
						}
						else if(c=='\\')
						{
							estado=3;
							t.lexema[++i]=c;
						}
						else if(c=='\"')
						{
							estado=5;
							t.lexema[++i]=c;
						}
						 else if(c==EOF)
						{
							estado=-1;
						}
						else {
							estado=-2;
						}	
						break;
					case(3):
						c=fgetc(archivo_fuente);
						if(is_scape(c))
						{
							estado=4;
							t.lexema[++i]=c;
						}
						else if(c!=EOF)
						{
							estado=-2;
						}
						else if(c==EOF)
						{
							estado=-1;
						}
						break;
					case(4):
						c=fgetc(archivo_fuente);
						if(c != '\\' && c!='\"')
						{
							estado=2;
							t.lexema[++i]=c;
						}
						else if(c=='\\')
						{
							estado=3;
							t.lexema[++i]=c;
						}
						else if(c=='\"')
						{
							estado=5;
							t.lexema[++i]=c;
						}
					  	else if(c==EOF)
					 	{
							estado=-1;
						}	
						break;
					case(5):
						estado_aceptacion = 1;
						t.lexema[++i]='\0';
						strcpy(t.complex,"STRING");
						c=0;
						break;	
					case(-1):
						printf("Error en la construccion de STRING en la linea %d \n",linea);;
						printf("No se esperaba fin de archivo");
                        consumir();
                        return;
					case(-2):
						printf("Error en la construccion de STRING en la linea %d\n",linea);
						printf("Caracter de escape no valido\n");
						printf("Se esperaba \\,/,b,u,f,n,r,t,u despues de %s  en cambio se encontro %c\n",t.lexema,c);
                        consumir();
                        return;
				}
			}	
		} 
	    else if (isdigit(c))//es un numero
	    { 
			int i=0;
			estado=1;
			estado_aceptacion=0;
			t.lexema[i]=c;
				
			while(!estado_aceptacion)
			{
				switch(estado)
				{
					case 1: //una secuencia netamente de digitos,un punto u other
						c=fgetc(archivo_fuente);   
						if (isdigit(c))
						{
							t.lexema[++i]=c;
							estado=1;
						}
						else if(c=='.')
						{
							t.lexema[++i]=c;
							estado=2;
						}
						else if (tolower(c)=='e')
						{
							t.lexema[++i]=c;
							estado=3;
						}
						else//other
						{
							estado=7;
						}
					break;
					case 2://luego del punto debe venir obligatoriamente un digito
						c=fgetc(archivo_fuente);						
						if (isdigit(c))
						{
							t.lexema[++i]=c;
							estado=4;
						}
						else
						{
							printf("Error lexico en la linea %d\n",linea);
							printf("Se esperaba un digito luego del punto, se encontro en cambio '%c'",c);
							estado=-1;
						}
					break;
					case 3://la parte decimal pude contener mas digitos, una E|e u other
						c=fgetc(archivo_fuente);
						if (c=='+')
						{
							t.lexema[++i]=c;
							estado=5;
						}
						else if(c=='-')
						{
							t.lexema[++i]=c;
							estado=5;
						}
						else if(isdigit(c))
						{
							t.lexema[++i]=c;
							estado=6;
						}
						else
						{
							printf("Error lexico en la linea %d",linea);
							printf("Se esperaba + - o un digito despues de exponente(e),se encontro en cambio '%c'",c);
							estado=-1;
						}
					
					break;
					case 4:// A la parte decimal pueden seguir varios digitos,un exponente(e) u other
						c=fgetc(archivo_fuente);						
						if (isdigit(c))
						{
							t.lexema[++i]=c;
							estado=4;
						}
						else if (tolower(c)=='e')
						{
							t.lexema[++i]=c;
							estado=3;
						}
						else //other
						{
							estado=7;
						}

					break;
					case 5://luego de singo + o - necesariamente debe venir por lo menos un digito
						c=fgetc(archivo_fuente);
						if (isdigit(c))
						{
							t.lexema[++i]=c;
							estado=6;
						}
						else
						{
							printf("Error lexico en la linea %d",linea);
							printf("Se esperaba un digito despues del signo,se encontro en cambio '%c'",c);
							estado=-1;
						}
					break;
					case 6://luego del primer digito seguido al signo del exponente(e+|e-) pueden venir mas digitos o un other
						c=fgetc(archivo_fuente);
						if (isdigit(c))
						{
							t.lexema[++i]=c;
							estado=6;
						}
						else // other
						{
							estado=7;
						}
					break;
					case 7://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
						{
							ungetc(c,archivo_fuente);
						}
						else
						{
							c=0;
						}

						t.lexema[++i]='\0';
						estado_aceptacion=1;
						strcpy(t.complex,"NUMBER");
						c=0;
					break;
					case -1:
						if (c==EOF)
						{
							printf("No se esperaba el fin de archivo");
						}
						else
						{
                            consumir();
                            return;
						}
				}
			}
		}
		else if(c==':')
		{
			t.lexema[0]=c;
			t.lexema[1]='\0';
			strcpy(t.complex,"DOS_PUNTOS");
		}
		else if(c==',')
		{
			t.lexema[0]=c;
			t.lexema[1]='\0';
			strcpy(t.complex,"COMA");
		}
		else if(c=='{')
		{
			t.lexema[0]=c;
			t.lexema[1]='\0';
			strcpy(t.complex,"L_LLAVE");
		}
		else if(c=='}')
		{
			t.lexema[0]=c;
			t.lexema[1]='\0';
			strcpy(t.complex,"R_LLAVE");
		}
		else if(c=='[')
		{
			t.lexema[0]=c;
			t.lexema[1]='\0';
			strcpy(t.complex,"L_CORCHETE");
		}
		else if(c==']')
		{
			t.lexema[0]=c;
			t.lexema[1]='\0';
			strcpy(t.complex,"R_CORCHETE");
		}
		else if(isalpha(c)) // es una palabra  reservada
		{
			int i=0;
			t.lexema[i]=c;
			while(isalpha(c))
			{
				c=fgetc(archivo_fuente);
				if(c!=EOF && isalpha(c))
				{
					t.lexema[++i]=c;
				}
				else
				{
					t.lexema[++i]='\0';
					ungetc(c,archivo_fuente);
					break;
				}
			}
			if(strcmp(t.lexema,"false")==0) //palabra reservada false
			{
				strcpy(t.complex,"PR_FALSE");
			}
			else if(strcmp(t.lexema,"true")==0)//palabra reservada true
			{
				strcpy(t.complex,"PR_TRUE");
			}
			else if(strcmp(t.lexema,"null")==0)//palabra reservada true
			{
				strcpy(t.complex,"PR_NULL");
			}
			else //no es una palabra reservada
			{
				printf("Error lexico en la linea %d\n",linea);
				printf("%s no es una palabra reservada",t.lexema);
                consumir();
                return;
			}
		}
        else
        {
            printf("\n error en la linea %d no se reconoce caracter %c \n",linea,c);
            consumir();
        }
	}
	
	if(c==EOF)
	{
		t.complex[0]=EOF;
	}
}

int is_scape(char c){
    char list_scape[10] = {'\"','\\','/','b','u','f','n','r','t','u'};
    int bandera=0;
    int i;
    for(i=0; i<9; i++){
      if(list_scape[i] == c){
       bandera = 1;
       break;                 
      }
    }
    return bandera;
}

void consumir()
{
    estado_error=1;
    while(c!='\n' && c!=EOF)
    {
        c=fgetc(archivo_fuente);
        if(c=='\n')
        {
            linea ++;
            fprintf(archivo_salida,"\n");
        }
       
    }
}
int main()
{
	if(!(archivo_fuente=fopen("fuente.txt","rt"))){
		printf("archivo no encontrado");
	}else
	{
		archivo_salida=fopen("resultado.txt","a");
		while(t.complex[0]!=EOF){
			sig_lex();
            // si no se encuentra en estado de error  y el componente lexico es un correcto y disinto a EOF se escribe en el archivo
			if(estado_error==0 && t.complex[0]!= EOF && isalpha(t.complex[0])){ 
				fprintf(archivo_salida," %s",t.complex);
			}
		}

	}
	fclose(archivo_fuente);
	return 0;
}

