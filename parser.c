#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAMLEX 1000

typedef struct
{
	char lexema[TAMLEX];
	char complex[20];
}Token;

//declaracion de funciones del lexer
int is_scape(char c);
void consumir();
//declaracion de funciones del parser
int  json();
int  array();
int  element();
int  array();
int  opcion_array();
int  element_list();
int  element_list_prima();
int  object();
int  opcion_object();
int  attributes_list();
int  attributes_list_prima();
int  attribute();
int  attribute_name();
int  attribute_value();
int  match();
void error();
Token t;
FILE *archivo_fuente;
//FILE *archivo_salida;
int estado_error;
char c;
int linea=1;
int co=0;
int error_encontrado = 0;
int err;
void sig_lex(){

	//co++;
	//printf("%d",co);
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
          // fprintf(archivo_salida,"%c",c);
		}else if(c=='\n')
		{
			linea++;
			//fprintf(archivo_salida,"%c",c);

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
						//exit(1);
                        consumir();
                        return;
					case(-2):
						printf("Error en la construccion de STRING en la linea %d\n",linea);
						printf("Caracter de escape no valido\n");
						printf("Se esperaba \\,/,b,u,f,n,r,t,u despues de %s  en cambio se encontro %c\n",t.lexema,c);
						//exit(1);
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
							//exit(1);
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
				//exit(1);
                consumir();
                return;
			}
		}
        else
        {
            printf("\n error en la linea %d no se reconoce caracter %c \n",linea,c);
            //exit(1);
            consumir();
        }
	}

	if(c==EOF)
	{
		strcpy(t.complex,"EOF");
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
   // printf("\nllamada nro %d \n",++err);
   // printf("el valor de c antes del while %c",c);
    while(c!='\n' && c!=EOF)
    {
       // printf("se comio la %c",c);
        c=fgetc(archivo_fuente);
        if(c=='\n')
        {
            linea ++;
            //fprintf(archivo_salida,"\n");
        }

    }
}

void get_token()
{
		sig_lex();
		while ((isalpha(t.complex[0])==0 || estado_error==1) && t.complex[0]!=EOF)
		{
			sig_lex();
			//fprintf(archivo_salida,t.lexema);
		}
}
// ***************funciones del analizador sintactico***************************
int json()
{
  element();
  match("EOF");
  return 0;
}

int element()
{
  if (strcmp(t.complex,"L_CORCHETE")==0)
  {
    array();
  }
  else if(strcmp(t.complex,"L_LLAVE")==0)
  {
    object();
  }
	else{
		error();
	}
  return 0;
}

int array()
{
  match("L_CORCHETE");
  opcion_array();
  return 0;
}

int opcion_array()
{
  if (strcmp(t.complex,"L_CORCHETE")==0 || strcmp(t.complex,"L_LLAVE")==0)
  {
    element_list();
    match("R_CORCHETE");
  }
  else if(strcmp(t.complex,"R_CORCHETE")==0)
  {
    match("R_CORCHETE");
  }
  else
  {
    error();
  }
  return 0;
}

int element_list()
{
  element();
  element_list_prima();
  return 0;
}

int element_list_prima()
{
  if (strcmp(t.complex,"COMA")==0)
	{
    match("COMA");
    element();
    element_list_prima();
  }
  //else puede no venir por el empty string
  return 0;
}

int object()
{
  match("L_LLAVE");
  opcion_object();
  return 0;
}

int opcion_object()
{
  if (strcmp(t.complex,"STRING")==0) {
    attributes_list();
		match("R_LLAVE");
  }
  else if(strcmp(t.complex,"R_LLAVE")==0)
  {
    match("R_LLAVE");
  }
  else
  {
      error();
  }
  return 0;
}

int  attributes_list()
{
  attribute();
  attributes_list_prima();
  return 0;
}

int  attributes_list_prima()
{
  if(strcmp(t.complex,"COMA")==0)
  {
    match("COMA");
    attribute();
    attributes_list_prima();
  }
  //else toma el empty string
  return 0;
}

int attribute()
{
  attribute_name();
  match("DOS_PUNTOS");
  attribute_value();
  return 0;
}

int attribute_name()
{
  match("STRING");
  return 0;
}

int attribute_value()
{
  if (strcmp(t.complex,"L_CORCHETE")==0 || strcmp(t.complex,"L_LLAVE")==0)
  {
    element();
  }
  else if(strcmp(t.complex,"STRING")==0)
  {
    match("STRING");
  }
  else if(strcmp(t.complex,"NUMBER")==0)
  {
    match("NUMBER");
  }
  else if(strcmp(t.complex,"PR_TRUE")==0)
  {
    match("PR_TRUE");
  }
  else if(strcmp(t.complex,"PR_FALSE")==0)
  {
    match("PR_FALSE");
  }
  else if(strcmp(t.complex,"PR_NULL")==0)
  {
    match("PR_NULL");
  }
  else
  {
    error();
  }

  return 0;
}

int match(char exp_token[20])
{
  if (strcmp(t.complex,exp_token)==0)
  {
		printf("\n matcheo: %s",t.complex);
		printf("\n");
    get_token();
	}
  else
  {
    error();
  }
  return 0;
}

void error(){
  printf("\n Error sintactico en la linea %d, no se esperaba %s",linea,t.complex);
	error_encontrado = 1;
  exit(1);
}

int main()
{
	if(!(archivo_fuente=fopen("fuente.txt","rt"))){
		printf("archivo no encontrado");
	}else
	{
    		get_token();
		json();
		if (error_encontrado == 0)
		{
			printf("\n");
			printf("Sin error");
		}
		fclose(archivo_fuente);
	}
	//getchar();
	return 0;
}
