// main.c
// 
// Copyright 2012 timmy <timmy@timmy-VirtualBox>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.
// 
// 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _AX 0
#define _AI 1
#define _BN 2
#define _B 3 
#define _BI 4
#define _CX 5
#define _DO 6
#define _DXY 7 
#define _DA 8
#define _DEX 9
#define _DT 10
#define _EX 11
#define _FXY 12
#define _I 13
#define _LB 14
#define _LT 15
#define _MB 16
#define _MI 17
#define _MT 18
#define _NXY 19
#define _NN 20
#define _PK 21
#define _R 22
#define _SI 23
#define _T 24
#define _TB 25
#define _TI 26
#define _TM 27
#define _TS 28
#define _TT 29
#define _US 30
#define _VX 31
#define _VBX 32
#define _Y 33
#define _Z 34
#define _NOT 35
#define _C 37
#define _N 39
#define _CONST 40
#define _NUMB 38
#define _UN 36
#define _FK 41

typedef struct table_data {
	char champs[50][8][64],
		 primary[16][64],
		 unique[16][64],
		 foreign[16][2][64],
		 engine[20],
		 nomtable[64];
	int e, champs_courant,
		cle_courante,
		unique_courant,
		foreign_courant;
} TableData;

void init_table_data(TableData* t) {
	int i,j;
	
	memset(t, 0, sizeof(struct table_data));
	t->champs_courant = -1;
	
	for (i=0; i<50; i++){
		for (j=0; j<8; j++){
			t->champs[i][j][0]='*';
		}
	}
}

TableData* new_table_data() {
	TableData* t = (TableData*)malloc(sizeof(struct table_data));
	init_table_data(t);
	return t;
}
	
void output(TableData t) {
	int i,j;
	// nom de la table, ouverture de la parenthèse
	printf("CREATE TABLE %s (", t.nomtable);
	// premier champ
	printf("\n\t");
	printf("%s %s", t.champs[0][0], t.champs[0][1]);
	if (t.champs[0][2][0] != '*') printf("%s", t.champs[0][2]);
	for (j=3; j<8; j++){
		if (t.champs[0][j][0] != '*') printf(" %s", t.champs[0][j]);
	}
	// tous les autres champs
	for (i=1; i<=t.champs_courant; i++){
		printf(",");
		printf("\n\t");
		printf("%s %s", t.champs[i][0], t.champs[i][1]);
		if (t.champs[i][2][0] != '*') printf("%s", t.champs[i][2]);
		for (j=3; j<8; j++){
			if (t.champs[i][j][0] != '*') printf(" %s", t.champs[i][j]);
		}
	}
	// clé primaire
	if (t.cle_courante>0){
		printf(",\n\tPRIMARY KEY(");
		for (i=0; i<t.cle_courante; i++){
			if (i!=0) printf(",");
			printf("%s", t.primary[i]);
		}
		printf(")");
	}
	// uniques
	if  (t.unique_courant>0){
		printf(",\n\tUNIQUE(");
		for (i=0; i<t.unique_courant; i++){
			if (i!=0) printf(",");
			printf("%s", t.unique[i]);
		}
		printf(")");
	}
	// clés étrangères
	if (t.foreign_courant>0){
		for (i=0; i<t.foreign_courant; i++){
			printf(",");
			printf("\n\tFOREIGN KEY %s REFERENCES %s", t.foreign[i][0], t.foreign[i][1]);
		}
	}
	// fermeture de la parenthèse
	printf("\n)");
	// engine
	if (t.e==1) printf("\nENGINE=%s;\n", t.engine);
	else printf(";\n");
}
	
void (*form)();

inline void one_line() { return; }
inline void multiple_lines() {	printf("\n"); }

void clean_line_feed(char* string) {
	char* ptr;
	if ((ptr=strchr(string, '\n'))!=NULL) {
		*ptr='\0';
	}
	else {
		if ((ptr=strchr(string, '\r'))!=NULL) {
			*ptr='\0';
		}
	}
}

int analyser_argument(const char* arg){
	if (arg[0] == ':'){
		switch(arg[1]){
			case 't' :
				switch(arg[2]){
					case 'b' : return _TB;
					case 'i' : return _TI;
					case 'm' : return _TM;
					case 's' : return _TS;
					case 't' : return _TT;
					default : return _T;
				}
			case 's' :
				if (arg[2] == 'i') return _SI;
				else return _SI;
			case 'm' :
				switch(arg[2]){
					case 'b' : return _MB;
					case 'i' : return _MI;
					case 't' : return _MT;
					default : return _NOT;
				}
			case 'i' : return _I;
			case 'b' :
				if (arg[2] == 'i') return _BI;
				else if (arg[2] == 'n') return _BN;
				else return _B;
			case 'u' :
				switch(arg[2]){
					case 's' : return _US;
					case 'n' : return _UN;
					default : return _NOT;
				}
			case 'a' :
				if (arg[2] == 'i') return _AI;
				else return _AX;
			case 'z' : return _Z;
			case 'n' :
				if (arg[2] == 'n') return _NN;
				else return _NXY;
			case 'd' :
				switch(arg[2]){
					case 'o' : return _DO;
					case 'a' : return _DA;
					case 'e' : return _DEX;
					case 't' : return _DT;
					default : return _DXY;
				}
			case 'f' :
				if (arg[2] == 'k') return _FK;
				else return _FXY;
			case 'r' : return _R;
			case 'c' : return _CX;
			case 'v' :
				if (arg[2] == 'b') return _VBX;
				else return _VX;
			case 'l' :
				if (arg[2] == 'b') return _LB;
				else return _LT;
			case 'y' : return _Y;
			case 'p' : return _PK;
			default : return _NOT;
		}
	}
	else if (arg[0] == '-'){
		switch(arg[1]){
			case 'e' : return _EX;
			case 'c' : return _C;
			case 'n' : return _N;
			default : return _NOT;
		}
	}
	else if (arg[0] == '\'') return _CONST;
	else return _NUMB;
	return _NOT;
}

int main(int argc, char **argv){
	// help
	if (argc == 2)
		if (argv[1][0]=='-' && argv[1][1]=='h') {
			printf("***** Programme de creation de table MySQL\n");
			printf("\nCe programme permet d'afficher sur la sortie standard.\n");
			printf("une table MySQL en utilisant des raccourcis,\n");
			printf("cela permet un gain de temps et une facilite d'utilisation.\n");
			printf("Le programme lit les parametres et cree la table en consequence.\n");
			printf("\nSyntaxe : %s -n <table> -c <champ1> :type [:complements] [-e 'ENGINE'] [-c <champ2> ...] [-n <table2> ...]\n", argv[0]);
			printf("\nOptions disponibles :\n");
			printf("\t -n <nom_de_la_table> (pour donner le nom de la table)\n");
			printf("\t -c <nom_du_champ> (pour creer un nouveau champ dans la table courante)\n");
			printf("\t -e <'moteur'> (pour specifier le moteur)\n\n");
			printf("* TYPE RACCOURCI\n");
			printf("* tinyint        :ti\n");
			printf("* smallint       :si\n");
			printf("* mediumint      :mi\n");
			printf("* int            :i\n");
			printf("* bigint         :bi\n");
			printf("* \n");
			printf("* numeric(x,y)   :n x,y\n");
			printf("* decimal(x,y)   :d x,y\n");
			printf("* float(x,y)     :f x,y\n");
			printf("* double         :do\n");
			printf("* real           :r\n");
			printf("* \n");
			printf("* char(x)        :c x\n");
			printf("* varchar(x)     :v x\n");
			printf("* \n");
			printf("* tinytext       :tt\n");
			printf("* text           :t\n");
			printf("* mediumtext     :mt\n");
			printf("* longtext       :lt\n");
			printf("* \n");
			printf("* varbinary(x)   :vb x\n");
			printf("* binary(x)      :bn x\n");
			printf("* tinyblob       :tb\n");
			printf("* blob           :b\n");
			printf("* mediumblob     :mb\n");
			printf("* longblob       :lb\n");
			printf("* \n");
			printf("* date           :da\n");
			printf("* datetime       :dt\n");
			printf("* time           :tm\n");
			printf("* timestamp      :ts\n");
			printf("* year           :y\n");
			printf("* \n");
			printf("* unsigned       :us\n");
			printf("* affichage(x)   :a x\n");
			printf("* zerofill       :z\n");
			printf("* not null       :nn\n");
			printf("* auto_increment :ai\n");
			printf("* primary key    :pk\n");
			printf("* foreign key    :fk TABLE ATTRIBUT\n");
			printf("* unique         :un\n");
			printf("* default        :de 'x'\n");
			printf("\n Exemple :\n");
			printf("%s -n T_ELEM_GAME -c t_egm_id :ti :ai :pk -c t_egm_title :v 20 :un -c t_egm_editor :v 20 -c t_egm_year :y -c t_egm_cover :i :us -c t_egm_desc :t\n", argv[0]);
			printf("Cette commande affichera la table suivante :\n\n");
			printf("CREATE TABLE T_ELEM_GAME (\n");
			printf("\tt_egm_id TINYINT AUTO_INCREMENT,\n");
			printf("\tt_egm_title VARCHAR(20),\n");
			printf("\tt_egm_editor VARCHAR(20),\n");
			printf("\tt_egm_year YEAR,\n");
			printf("\tt_egm_cover INT UNSIGNED,\n");
			printf("\tt_egm_desc TEXT,\n");
			printf("\tPRIMARY KEY(t_egm_id),\n");
			printf("\tUNIQUE(t_egm_title)\n");
			printf(");\n");
			printf("\nL'ordre dans lequel vous entrez les parametres n'a pas d'importance.\n");
			printf("Le programme ecrit les valeurs dans l'ordre correspondant a la syntaxe MySQL :\n");
			printf("type(x) zerofill unsigned not null default 'CONST' auto_increment\n");
			printf("Le programme respecte les specifications MySQL :\n");
			printf("Les noms de table/champs/cles ne depassent pas 64 caracteres.\n");
			printf("On peut preciser au maximum 50 champs, 16 cles primaires,\n");
			printf("16 cles etrangeres, et 16 uniques par table.\n");
			printf("\nLimitations :\n");
			printf("Il n'est pas possible de preciser plusieurs UNIQUE distincts :\n");
			printf("Si vous entrez plusieurs arguments \":un\", le champ UNIQUE\n");
			printf("sera une combinaison des champs pour lesquels vous l'avez precise.\n");
			printf("Il n'est pas possible de donner directement un nom aux contraintes\n");
			printf("d'integrite referentielle dans la ligne de commande: il faudra\n");
			printf("modifier le fichier de sortie. Pour ecrire les resultats dans un\n");
			printf("fichier au lieu de les afficher sur stdout, utilisez une redirection !\n");
			return 0;
		}
	
	// usage
	if (argc < 5 /*|| strcmp(argv[1], "-n")*/){
		fprintf(stderr, "usage: %s -n <table1> -c <champ1> :type [:complements] [-e 'ENGINE'] [-c <champ2> ...] [-n <table2> ...]\n", argv[0]);
		fprintf(stderr, "Entrez %s -h pour afficher l'aide\n", argv[0]);
		return 1;
	}
	
	// déclarations et initialisations
	int i,arg;
	TableData* t = NULL;
	
	clean_line_feed(argv[argc-1]);	
	
	// analysis
	for(i=1; i<argc; i++){
		arg = analyser_argument(argv[i]);
		switch(arg){
			case _AX : // affichage
				sprintf(t->champs[t->champs_courant][2],"(%s)", argv[i+1]);
				i++;
				break;
			case _AI :
				sprintf(t->champs[t->champs_courant][7],"AUTO_INCREMENT");
				break;
			case _BN :
				sprintf(t->champs[t->champs_courant][1],"BINARY(%s)", argv[i+1]);
				i++;
				break;
			case _B :
				sprintf(t->champs[t->champs_courant][1],"BLOB");
				break;
			case _BI :
				sprintf(t->champs[t->champs_courant][1],"BIGINT");
				break;
			case _CX : 
				sprintf(t->champs[t->champs_courant][1],"CHAR(%s)", argv[i+1]);
				i++;
				break;
			case _DO :
				sprintf(t->champs[t->champs_courant][1],"DOUBLE");
				break;
			case _DXY :
				sprintf(t->champs[t->champs_courant][1],"DECIMAL(%s)", argv[i+1]);
				i++;
				break;
			case _DA :
				sprintf(t->champs[t->champs_courant][1],"DATE");
				break;
			case _DEX :
				sprintf(t->champs[t->champs_courant][6],"DEFAULT '%s'", argv[i+1]);
				i++;
				break;
			case _DT :
				sprintf(t->champs[t->champs_courant][1],"DATETIME");
				break;
			case _EX : // engine
				clean_line_feed(argv[i+1]);
				sprintf(t->engine,"%s",argv[i+1]);
				t->e=1;
				i++;
				break;
			case _FK :
				sprintf(t->foreign[t->foreign_courant][0],"(%s)", t->champs[t->champs_courant][0]);
				sprintf(t->foreign[t->foreign_courant][1],"%s(%s)", argv[i+1], argv[i+2]);
				t->foreign_courant++;
				break;
			case _FXY :
				sprintf(t->champs[t->champs_courant][1],"FLOAT(%s)", argv[i+1]);
				i++;
				break;
			case _I :
				sprintf(t->champs[t->champs_courant][1],"INT");
				break;
			case _LB :
				sprintf(t->champs[t->champs_courant][1],"LONGBLOB");
				break;
			case _LT :
				sprintf(t->champs[t->champs_courant][1],"LONGTEXT");
				break;
			case _MB :
				sprintf(t->champs[t->champs_courant][1],"MEDIUMBLOB");
				break;
			case _MI :
				sprintf(t->champs[t->champs_courant][1],"MEDIUMINT");
				break;
			case _MT :
				sprintf(t->champs[t->champs_courant][1],"MEDIUMTEXT");
				break;
			case _NXY :
				sprintf(t->champs[t->champs_courant][1],"NUMERIC(%s)", argv[i+1]);
				i++;
				break;
			case _NN :
				sprintf(t->champs[t->champs_courant][5],"NOT NULL");
				break;
			case _PK : // clé primaire
				sprintf(t->primary[t->cle_courante],"%s", t->champs[t->champs_courant][0]);
				t->cle_courante++;
				break;
			case _R :
				sprintf(t->champs[t->champs_courant][1],"REAL");
				break;
			case _SI :
				sprintf(t->champs[t->champs_courant][1],"SMALLINT");
				break;
			case _T :
				sprintf(t->champs[t->champs_courant][1],"TEXT");
				break;
			case _TB :
				sprintf(t->champs[t->champs_courant][1],"TINYBLOB");
				break;
			case _TI :
				sprintf(t->champs[t->champs_courant][1],"TINYINT");
				break;
			case _TM :
				sprintf(t->champs[t->champs_courant][1],"TIME");
				break;
			case _TS :
				sprintf(t->champs[t->champs_courant][1],"TIMESTAMP");
				break;
			case _TT :
				sprintf(t->champs[t->champs_courant][1],"TINYTEXT");
				break;
			case _US :
				sprintf(t->champs[t->champs_courant][4],"UNSIGNED");
				break;
			case _UN :
				sprintf(t->unique[t->unique_courant],"%s", t->champs[t->champs_courant][0]);
				t->unique_courant++;
				break;
			case _VX :
				sprintf(t->champs[t->champs_courant][1],"VARCHAR(%s)", argv[i+1]);
				i++;
				break;
			case _VBX :
				sprintf(t->champs[t->champs_courant][1],"VARBINARY(%s)", argv[i+1]);
				i++;
				break;
			case _Y :
				sprintf(t->champs[t->champs_courant][1],"YEAR");
				break;
			case _Z :
				sprintf(t->champs[t->champs_courant][3],"ZEROFILL");
				break;
			case _NOT :
				break;
			case _C : // nouveau champ
				t->champs_courant++;
				sprintf(t->champs[t->champs_courant][0],"%s", argv[i+1]);
				i++;
				break;
			case _N : // nouvelle table
				if (t==NULL)
					t=new_table_data();
				else {
					output(*t);
					init_table_data(t);
				}
				sprintf(t->nomtable,"%s", argv[i+1]);
				i++;
				break;
			case _CONST :
				break;
			case _NUMB :
				break;
		}
	}
	
	// mise en forme
	if (t!=NULL) {
		output(*t);
		free(t);
	}
	
	return 0;
}
