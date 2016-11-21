/*
File Name 		: cardinality2.cpp
Author			: Harshvardhan Gazula
Modified On 	: 08/15/2016
*/

#include <ilcplex/cplex.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <sstream>
#include <iomanip> // for setfill and setw
using namespace std;

int curr_run = 0;
int D = 0; // i added to account for density

bool ONE_TEST = false;
bool DENSE_ST = false;

int nmbsec = 3600;  // changed 3 hours = 10800 seconds to 1 hour = 3600 seconds

int N = 0;
int M = 0;
int K = 0;
int myCount = 0;
int solstat = 0;

int*		init_C;
int*		init_N_1; 
int*		orderC_ax; 
int*		orderC_a; 
int*		cover; 
int*		N_1; 
double*		cutval; 
double*		node_slack; 
int*		cutind; 

#define EPGAP	0.0001 // 0.00055
#define EPS		1e-06
#define DBL_MAX	100000

int nmbcll = 2000000;
int nmbct1 = 1;
int nmbct2 = 0;
int nmbsp1 = 0;
int nmbsp2 = 0;

int option = 0; // 0 no option
				// 1 cut generation controlled by current gap
				// 2 cut generation controlled by number of nodes
				// 3 cut generation at root node only + CPLEX branch-and-cut (if mipcut == true, branch-and-bound otherwise )
				// 4 cut generation at root node only + cut printing
				// 5 cut genetation at the first numgnt nodes
				// 6 dense cut generation controlled by number of dense cuts (nmbct1) and sparse cut generation controlled by number of sparse cuts (nmbsp1 + nmbsp2)

int cntcll = 0;
int cntct1 = 0;
int cntct2 = 0;
int cntsp1 = 0;
int cntsp2 = 0;

int cntgap = 0; // optiont = 1 
int numgap = 10000; 
double crr_gap = 100;

int cntnds = 1000; // optiont = 2 

double numgnt = 20; // optiont = 5 

//double* ord;

int last_ct1 = -1;
int last_sp1 = -1;
int last_ct2 = -1;
int last_sp2 = -1;

int num_nodes = 0;
int num_flowc = 0;
int num_gomoc = 0;
int num_userc = 0;
double num_secnd = 0;
double val_soltn = 0;

bool mipcut = true;
bool presolve = true;
bool weak = false;
bool y_rpl_x = false;

FILE *fout;

struct cutinfoplus {
	int		numcols;
	double**	a;
	double*		b;
	double*		obj;
	int*		rel;
	double*		s;
	int*		clm_y;	
};

typedef struct cutinfoplus CUTINFOplus, *CUTINFOptr;

void set_calldata()
{
	init_C 		= new int[N];
	init_N_1 	= new int[N];
	orderC_ax 	= new int[K];
	orderC_a 	= new int[K];
	cover 		= new int[N];
	N_1 		= new int[N];
	cutval 		= new double[N];
	node_slack 	= new double[M];
	cutind 		= new int[N];
}

void del_calldata()
{
	delete[] init_C;
	delete[] init_N_1 ;
	delete[] orderC_ax ;
	delete[] orderC_a ;
	delete[] cover;
	delete[] N_1;
	delete[] cutval;
	delete[] node_slack ;
	delete[] cutind ;
}

void set_cutinfo(CUTINFOplus& c)
{
	int i;

	c.numcols 	= 0;
	c.a 		= new double*[M]; 
	for( i = 0; i < M; i ++) 
		c.a[i] 	= new double[N]; 
	c.b  		= new double[M]; 
	c.obj  		= new double[N]; 
	c.rel  		= new int[N]; 
	c.s  		= new double[N]; 
	c.clm_y  	= new int[N]; 
}
	
void del_cutinfo(CUTINFOplus& c)
{
	int i;

	c.numcols = 0;
	for( i = 0; i < M; i ++)
		delete[] c.a[i];
	delete[] c.a ;
	delete[] c.b ;
	delete[] c.obj ;
	delete[] c.rel ;
	delete[] c.s ;
	delete[] c.clm_y ;
}

void reset_all()
{
	cntcll = 0;
	cntct1 = 0;
	cntct2 = 0;
	cntsp1 = 0;
	cntsp2 = 0;

	last_ct1 = -1;
	last_sp1 = -1;
	last_ct2 = -1;
	last_sp2 = -1;

	num_nodes = 0;
	num_flowc = 0;
	num_gomoc = 0;
	num_userc = 0;
	num_secnd = 0;
}

static int CPXPUBLIC
	mycutcallback (CPXCENVptr	env,
	               void		*cbdata,
			   int		wherefrom,
			   void		*cbhandle,
			   int		*useraction_p);



static void
	free_and_null (char **ptr);

int main_1 (int argc, char **argv){
	
	char curr_run_str[2];	
	curr_run++;
	sprintf(curr_run_str, "%02d", curr_run);	
		
	reset_all();

	time_t rawtime;
	time ( &rawtime );

	char* str1 = ctime (&rawtime);
	char str2[30];
	char str3[3];
	char str4[3];
	char str5[3];
	char strd[4];
	char strn[3];
	strncpy (str2,str1,20);
	str2[19]='\0';
	strncpy (strd,str2,4);
	strd[3]='\0';
	strncpy (strn,str2+8,4);
	strn[2]='\0';
	strncpy (str2,str1+11,20);
	str2[19]='\0';
	strncpy (str3,str2,2);
	str3[2]='\0';
	strncpy (str4,str2+3,2);
	str4[2]='\0';
	strncpy (str5,str2+6,2);
	str5[2]='\0';

//	char log_name[27];
//	strncpy(log_name,"log_",4);
//	strncpy(&log_name[4],strd,3);
//	strncpy(&log_name[7],"_",1);
//	strncpy(&log_name[8],strn,2);
//	strncpy(&log_name[10],"_",1);
//	strncpy(&log_name[11],str3,2);
//	strncpy(&log_name[13],"_",1);
//	strncpy(&log_name[14],str4,2);
//	strncpy(&log_name[16],"_",1);
//	strncpy(&log_name[17],str5,2);
//	strncpy(&log_name[19],"_",1);
//	strncpy(&log_name[20],curr_run_str,2);
//	strncpy(&log_name[22],".txt",4);
//	log_name[26] = '\0';


//	char out_name[27];
//	strncpy(out_name,"out_",4);
//	strncpy(&out_name[4],strd,3);
//	strncpy(&out_name[7],"_",1);
//	strncpy(&out_name[8],strn,2);
//	strncpy(&out_name[10],"_",1);
//	strncpy(&out_name[11],str3,2);
//	strncpy(&out_name[13],"_",1);
//	strncpy(&out_name[14],str4,2);
//	strncpy(&out_name[16],"_",1);
//	strncpy(&out_name[17],str5,2);
//	strncpy(&out_name[19],"_",1);
//	strncpy(&out_name[20],curr_run_str,2);
//	strncpy(&out_name[22],".txt",4);
//	out_name[26] = '\0';	

	char str_prb[50];

	if( argc <= 1 )
	{
		printf("Enter the problem: ");
//		gets(str_prb);
		cin >> str_prb;
		printf("The problem is %s\n", str_prb);
	}
		
		// my new code starts here		
		string out_name, log_name, str_N, str_M, str_D, str_myCount, str_curr_run;
		stringstream convert;
		convert << setfill('0') << setw(4) << N;
		str_N = convert.str();
		convert.str("");
		convert << setfill('0') << setw(4) << M;
		str_M = convert.str();
		convert.str("");
		convert << setfill('0') << setw(4) << D;
		str_D = convert.str();
		convert.str("");
		convert << setfill('0') << setw(2) << myCount;
		str_myCount = convert.str();
		convert.str("");
		convert << setfill('0') << setw(2) << curr_run;
		str_curr_run = convert.str();

		out_name.append(".//Output//");
		out_name.append("out_");		
		out_name.append(str_N);
		out_name.append("_");
		out_name.append(str_M);
		out_name.append("_");
		out_name.append(str_D);
		out_name.append("_");
		out_name.append(str_myCount);
		out_name.append("_");
		out_name.append(str_curr_run);
		out_name.append(".txt");
		
		log_name.append(".//Logs//");
		log_name.append("log_");		
		log_name.append(str_N);
		log_name.append("_");
		log_name.append(str_M);
		log_name.append("_");
		log_name.append(str_D);
		log_name.append("_");
		log_name.append(str_myCount);
		log_name.append("_");
		log_name.append(str_curr_run);
		log_name.append(".txt");
		
		// my new code ends here
		
	int status = 0;

//	int    solstat;
	double objval; 
	double *x = NULL;
	int cur_numcols;
	int nodecount;
	int nodeflowcut;
	int nodegomorycut;
	int nodeusercut;
	int colindex;
	char str[10];

    char          **cur_colname = NULL;
    char          *cur_colnamestore = NULL;
//    int           cur_colnamespace;
//    int           surplus;

//	FILE *fout;

	CPXENVptr env = NULL;
	CPXLPptr  lp = NULL;
//	CPXFILEptr fp = CPXfopen ( log_name , "w"); // ("log.log", "w");
//	CPXFILEptr fp = CPXfopen ( log_name.c_str() , "w"); // ("log.log", "w");

//	CUTINFO cutinfo;
	CUTINFOplus cutinfo;

	set_cutinfo(cutinfo);
	
	//fout = fopen (out_name, "wt"); // "out.txt", "wt");  
//	fout = fopen (out_name.c_str(), "wt"); // "out.txt", "wt");  
//	
//	if( argc <= 1 )
//		fprintf (fout, "Problem : %s \n", str_prb);
//	else		
//		fprintf (fout, "Problem : %s \n", argv[1]);
//
//	fprintf (fout, "option : %d \n", option);
//	fprintf (fout, "presolve : %d \n", (int)presolve);
//	fprintf (fout, "mip cuts : %d \n", (int)mipcut);
//	fprintf (fout, "y variables cuts : %d \n", (int)y_rpl_x);
//	fprintf (fout, "weak cuts: %d \n", (int)weak);
//	fprintf (fout, "Number of cuts of type d1: %d  \n", nmbct1);
//	fprintf (fout, "Number of cuts of type s1: %d  \n", nmbsp1);
//	fprintf (fout, "Number of cuts of type d2: %d  \n", nmbct2);
//	fprintf (fout, "Number of cuts of type s2: %d  \n", nmbsp2);
//	
//	fflush ( fout );
	
// Initialize CPLEX and set some parameters ------------------------------------------------

	env = CPXopenCPLEX(&status);
	if (env == NULL) {
		char errmsg[1024];
		fprintf(stderr,"Could not open CPLEX enviroment. \n");
		CPXgeterrorstring (env, status, errmsg);
		fprintf (stderr, "%s", errmsg);
		goto TERMINATE;
	}

// newly added
   status = CPXsetintparam (env, CPX_PARAM_CLOCKTYPE, 1);
	if ( status ) {
		fprintf (stderr, "Failure to turn on screen indicator, error %d.\n", status);
		goto TERMINATE;
	}
//newly added

    status = CPXsetintparam (env, CPX_PARAM_SCRIND, CPX_ON);
	if ( status ) {
		fprintf (stderr, "Failure to turn on screen indicator, error %d.\n", status);
		goto TERMINATE;
	}

//    status = CPXsetlogfile (env, fp);
 	status = CPXsetlogfile (env, NULL);
	if ( status ) {
		fprintf (stderr, "Failure to set CPLEX log file, error %d.\n", status);
		goto TERMINATE;
	}

	if( argc <= 1 )
		lp = CPXcreateprob (env, &status, str_prb);
	else
		lp = CPXcreateprob (env, &status, argv[1]);
	   
	if ( lp == NULL ) {
		fprintf (stderr, "Failed to create LP.\n");
		goto TERMINATE;
	}

	if( argc <= 1 )
		status = CPXreadcopyprob (env, lp, str_prb, NULL);
	else
		status = CPXreadcopyprob (env, lp, argv[1], NULL);
	if ( status ) {
		fprintf (stderr,
				"Failed to read and copy the problem data.\n");
		goto TERMINATE;
	}

	status = CPXsetintparam (env, CPX_PARAM_PRELINEAR, 0);
	if (status) goto TERMINATE;

	if( !presolve )
	{
		status = CPXsetintparam (env, CPX_PARAM_PREIND, 0);
		if (status) goto TERMINATE;
	}

	if( nmbsec >= 1 )
	{
		status = CPXsetdblparam (env, CPX_PARAM_TILIM, nmbsec);
		if (status) goto TERMINATE;
	}

	status = CPXsetintparam (env, CPX_PARAM_MIPCBREDLP, CPX_OFF);
	if ( status )  goto TERMINATE;

	status = CPXsetdblparam (env, CPX_PARAM_EPGAP, EPGAP);
	if ( status )  goto TERMINATE;

	if( !mipcut )
	{
		status = CPXsetintparam (env, CPX_PARAM_FRACCUTS, -1);
		if ( status )  goto TERMINATE; 

		status = CPXsetintparam (env, CPX_PARAM_FLOWCOVERS, -1);
		if ( status )  goto TERMINATE;

		status = CPXsetintparam (env, CPX_PARAM_FLOWPATHS, -1);
		if ( status )  goto TERMINATE;

		status = CPXsetintparam (env, CPX_PARAM_MIRCUTS, -1);
		if ( status )  goto TERMINATE;

		status = CPXsetintparam (env, CPX_PARAM_CLIQUES, -1);
		if ( status )  goto TERMINATE;
	}

	status = CPXsetintparam (env, CPX_PARAM_MIPINTERVAL, 100);                       
	if (status) goto TERMINATE;

// -----------------------------------------------------------------------------------------

// initiate cutinfo ------------------------------------------------------------------------

	cur_numcols = CPXgetnumcols(env, lp);

	cutinfo.numcols = cur_numcols;
	
	for (int i=0; i<M; i++) {
		cutinfo.b[i] = 0.0;
		for (int j=0; j<N; j++)
			cutinfo.a[i][j] = -1.0;
	}
	
	str[0]='x';

	for (int i=0; i<M; i++) {
		for (int j=0; j<N; j++) {
			sprintf(str+1, "%d", j);
			CPXgetcolindex(env, lp, str, &colindex);
			if (i==0) cutinfo.rel[j] = colindex;			
			CPXgetcoef(env, lp, i, colindex, &cutinfo.a[i][j]);
		}
	}
	
	str[0]='y';

	for (int j=0; j<N; j++) {
		sprintf(str+1, "%d", j);
		CPXgetcolindex(env, lp, str, &colindex);
		cutinfo.clm_y[j] = colindex;
		if( cutinfo.clm_y[j] >= 2 * N ) printf( " %d ", cutinfo.clm_y[j]);
	}			
	
	status = CPXgetobj(env, lp, cutinfo.obj, 0, N-1);
	if (status) goto TERMINATE;

	CPXgetrhs(env, lp, cutinfo.b, 0, M - 1);
// -----------------------------------------------------------------------------------------

// solve problem and show the solution information -----------------------------------------

	status = CPXsetusercutcallbackfunc (env, mycutcallback, &cutinfo);
	if (status) goto TERMINATE; 

	clock_t begin, end;
	double difftime;

//	time_t begin, end;
	begin = clock();
//	begin = time(NULL);

	status = CPXmipopt(env, lp);
	if (status) {
		fprintf (stderr, "Failed to optimize MIP. \n");
		goto TERMINATE;
	}
	end = clock();
	difftime = ((double) (end - begin)) / CLOCKS_PER_SEC;
//	end = time(NULL);
//	difftime = ((double) (end - begin));

	solstat = CPXgetstat (env, lp);
	printf ("Solution status %d.\n", solstat);

	status = CPXgetmipobjval (env, lp, &objval);
	if ( status ) {
		fprintf (stderr,"Failed to obtain objective value.\n");
		goto TERMINATE;
	}

	printf ("Objective value %.10g\n", objval);

	/* Allocate space for solution */

	x = (double *) malloc (cur_numcols * sizeof (double));

	if ( x == NULL ) {
		fprintf (stderr, "No memory for solution values.\n");
		goto TERMINATE;
	}

	status = CPXgetmipx (env, lp, x, 0, cur_numcols-1);
	if ( status ) {
		fprintf (stderr, "Failed to obtain solution.\n");
		goto TERMINATE;
	}

/*
    status = CPXgetcolname (env, lp, NULL, NULL, 0, &surplus, 0,
                            cur_numcols-1);

    if (( status != CPXERR_NEGATIVE_SURPLUS ) &&
        ( status != 0 )                         )  {
       fprintf (stderr, 
                "Could not determine amount of space for column names.\n");
       goto TERMINATE;
    }

    cur_colnamespace = - surplus;
    if ( cur_colnamespace > 0 ) {
       cur_colname      = (char **) malloc (sizeof(char *)*cur_numcols);
       cur_colnamestore = (char *)  malloc (cur_colnamespace);
       if ( cur_colname      == NULL ||
            cur_colnamestore == NULL   ) {
          fprintf (stderr, "Failed to get memory for column names.\n");
          status = -1;
          goto TERMINATE;
       }
       status = CPXgetcolname (env, lp, cur_colname, cur_colnamestore, 
                               cur_colnamespace, &surplus, 0, cur_numcols-1);
       if ( status ) {
          fprintf (stderr, "CPXgetcolname failed.\n");
          goto TERMINATE;
       }
    }
    else {
       printf ("No names associated with problem.  Using Fake names.\n");
    }



    for (int j = 0; j < cur_numcols; j++) {
		if ( x[j] > 1e-6) {
			if ( cur_colnamespace > 0 ) {
				printf ("%-16s:  ", cur_colname[j]);
			}
			else {
				printf ("Fake%-6.6d      :  ", j);;
			}
			printf ("%17.10g", x[j]);
			printf ("\n");
		}
    }

	double sumobj;
	sumobj = 0.0;

	for (int i=0; i<N; i++) {
		sumobj+=x[i]*cutinfo.obj[i];
	}

	printf ("Objective value real %.10g\n", sumobj);
*/

//	CPXfclose (fp);

	nodecount = CPXgetnodecnt (env, lp);
	
	CPXgetnumcuts(env, lp, CPX_CUT_FLOWCOVER , &nodeflowcut);
	CPXgetnumcuts(env, lp, CPX_CUT_FRAC , &nodegomorycut);
	CPXgetnumcuts(env, lp, CPX_CUT_USER , &nodeusercut);

	printf ("option : %d \n", option);
	printf ("presolve : %d \n", (int)presolve);
	printf ("mip cuts : %d \n", (int)mipcut);
	printf ("y variables cuts : %d \n", (int)y_rpl_x);
	printf ("weak cuts: %d \n", (int)weak);
	printf ("Cuts of type d1: %d ( %d , %d ) \n", cntct1, nmbct1, last_ct1);
	printf ("Cuts of type s1: %d ( %d , %d ) \n", cntsp1, nmbsp1, last_sp1);
	printf ("Cuts of type d2: %d ( %d , %d ) \n", cntct2, nmbct2, last_ct2);
	printf ("Cuts of type s2: %d ( %d , %d ) \n", cntsp2, nmbsp2, last_sp2);
	printf ("Node: %d \n",nodecount);
    printf ("Solution time : %g\n", difftime);

//	if( argc <= 1 )
//		fprintf (fout, "Problem %s \n", str_prb);
//	else
//		fprintf (fout, "Problem %s \n", argv[1]);

/*	fprintf (fout, "presolve : %d \n", (int)presolve);
	fprintf (fout, "mip cuts : %d \n", (int)mipcut);
	fprintf (fout, "y variables cuts : %d \n", (int)y_rpl_x);
	fprintf (fout, "weak cuts: %d \n", (int)weak); */
//	fprintf (fout, "Cuts of type d1: %d ( %d ) \n", cntct1, nmbct1);
//	fprintf (fout, "Cuts of type s1: %d ( %d ) \n", cntsp1, nmbsp1);
//	fprintf (fout, "Cuts of type d2: %d ( %d ) \n", cntct2, nmbct2);
//	fprintf (fout, "Cuts of type s2: %d ( %d ) \n", cntsp2, nmbsp2);
//	fprintf (fout, "Node: %d \n",nodecount);
//	fprintf (fout, "Flow cuts: %d \n",nodeflowcut);
//	fprintf (fout, "Gomory cuts: %d \n",nodegomorycut);
//	fprintf (fout, "User cuts: %d \n",nodeusercut);
//  fprintf (fout, "Solution time : %g\n", difftime);
//	fprintf (fout, "Solution status %d.\n", solstat);
//	fprintf (fout, "Objective value %.10g\n", objval);
//	fclose (fout);  

	num_nodes = nodecount;
	num_flowc = nodeflowcut;
	num_gomoc = nodegomorycut;
	num_userc = nodeusercut;
	num_secnd = difftime;
	val_soltn = objval;

// -----------------------------------------------------------------------------------------

TERMINATE:
	set_cutinfo(cutinfo);
	free_and_null ((char **) &x);

	if ( lp != NULL ) {
		status = CPXfreeprob (env, &lp);
		if ( status ) {
			fprintf (stderr, "CPXfreeprob failed, error code %d.\n",
					status);
		}
	}

	if ( env != NULL ) {
		status = CPXcloseCPLEX (&env);

		if ( status ) {
			char errmsg[1024];
			fprintf (stderr, "Could not close CPLEX environment.\n");
			CPXgeterrorstring (env, status, errmsg);
			fprintf (stderr, "%s", errmsg);
		}
	}
  
	return (status);

} /* END main */

static void
free_and_null (char **ptr)
{
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
} /* END free_and_null */ 


static int CPXPUBLIC 
	mycutcallback (CPXCENVptr	env,
	               void			*cbdata,
				   int			wherefrom,
				   void			*cbhandle,
				   int			*useraction_p)
{	
	int status = 0;

	CUTINFOptr	cutinfo = (CUTINFOptr) cbhandle;
	CPXLPptr	nodelp;

	double		*x = NULL;
	double		*y = NULL;
//	double*		node_slack; // [M];
	int			numnods;
	int			numcols;
/*	int*		init_C; // [N],
	int*		init_N_1; // [N],
	int*		orderC_ax; // [K],
	int*		orderC_a; // [K],
	int*		cover; // [N],
	int*		N_1; // [N]; // ,cover1[N],cover2[N];
*/	int			num_Init_N_1,num_Init_C;
	double		a_C;
	double		cutvio,lambda,rhs,delta, new_gap, crr_sol, crr_bst; 
/*	double*		cutval; // [N]
	int*		cutind; // [N];
*/
	int lclct1 = 0;
	int lclct2 = 0;

	*useraction_p = CPX_CALLBACK_DEFAULT;

	if (cntcll >= nmbcll) goto TERMINATE;

	if (cntct1 >= nmbct1 && cntct2 >= nmbct2 && cntsp1 >= nmbsp1 && cntsp2 >= nmbsp2 ) goto TERMINATE;

	// 6 dense cut generation controlled by number of dense cuts (nmbct1) and sparse cut generation controlled by number of sparse cuts (nmbsp1 + nmbsp2)

	if( option == 6 )
	{
		if( cntct1 >= nmbct1 && cntct2 >= nmbct2 )
			weak = true;
		else
			weak = false;
	}

	cntcll++;
	
	numcols = cutinfo->numcols;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Get current node value of x

	x = (double *) malloc (numcols * sizeof (double));
	if ( x == NULL ) {
		fprintf (stderr, "No memory for solution values.\n");
		goto TERMINATE;
    }
	y = (double *) malloc (2*numcols * sizeof (double));
	if ( x == NULL ) {
		fprintf (stderr, "No memory for solution values.\n");
		goto TERMINATE;
    }

	status = CPXgetcallbacknodex (env, cbdata, wherefrom, y, 0, 2*N-1);
	if ( status ) {
		fprintf (stderr, "Failed to obtain solution.\n");
		goto TERMINATE;
	}

	status = CPXgetcallbackinfo(env, cbdata, wherefrom, CPX_CALLBACK_INFO_BEST_REMAINING, &crr_sol );
	if ( status ) {
		fprintf (stderr, "Failed to obtain current solution.\n");
		goto TERMINATE;
	}

	status = CPXgetcallbackinfo(env, cbdata, wherefrom, CPX_CALLBACK_INFO_BEST_INTEGER, &crr_bst );
	if ( status ) {
		fprintf (stderr, "Failed to obtain current best integer.\n");
		goto TERMINATE;
	}

	status = CPXgetcallbackinfo(env, cbdata, wherefrom, CPX_CALLBACK_INFO_NODE_COUNT, &numnods );
	if ( status ) {
		fprintf (stderr, "Failed to obtain current best integer.\n");
		goto TERMINATE;
	}

	new_gap = 100 * (double)(crr_sol - crr_bst) / crr_bst;

	if( option == 1 && numnods == 0 )
	{
		crr_gap = new_gap;
	}

	if( crr_gap >= new_gap + 0.01 ) // option 1
	{
		if( numnods >= 1 )
		{
			cntgap = 0;
			crr_gap = new_gap;
		}
	}
	else
	{
		cntgap++;

		if( option == 1 && cntgap <= numgap && numnods >= 1 )
			goto TERMINATE;
	}

	if( option == 2 && !( numnods % cntnds ) ) // option 2
		goto TERMINATE;

	if( option == 3 && numnods >= 1 ) // option 3
	{
		if( numnods == 1 )
			printf(" number of cuts %d \n",cntct1);

		goto TERMINATE;
	}

	for (int i=0; i<N; i++) x[i]=y[cutinfo->rel[i]];
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	status = CPXgetcallbacknodelp (env, cbdata, wherefrom, &nodelp);
	if ( status )  goto TERMINATE;

	status = CPXgetslack (env, nodelp, node_slack, 0, M-1);
	if ( status )  goto TERMINATE;

//	if( numnods == 1 )
//		status = CPXwriteprob (env, nodelp, "myprob_1.lp", NULL);

	if( option == 4 && numnods >= 1 ) // option 4
	{
		if( numnods == 1 )
			status = CPXwriteprob (env, nodelp, "myprob.lp", NULL);
		goto TERMINATE;
	}

	if( option == 5 && numnods >= numgnt )
		goto TERMINATE;

//	printf(" working %f - %f, %d \n",crr_gap,new_gap,cntgap);
	
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// code is for every tight row.	

	for (int i=0; i<M; i++) {
		if (node_slack[i] > 0.0) continue;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// initiate variables

		num_Init_N_1 = 0; num_Init_C = 0; lambda = -cutinfo->b[i]; a_C = DBL_MAX;

		for (int j=0; j<N; j++) {
			init_C[j] = 0; init_N_1[j] = 0; 
		}
		
		for (int j=0; j<K; j++) {
			orderC_ax[j] = j;
			orderC_a[j] = j;
		}
		
//		//my code
//		
//		for (int j=0; j<N; j++) {
//			printf("a[%d] = %f, x[%d] = %f, C[%d] = %d, N1[%d] = %d\n", j, cutinfo->a[i][j], j, x[j], j, init_C[j], j, init_N_1[j]); 			
//		}
//		printf("b[%d] = %f\n", i, cutinfo->b[i]);
//		//my code
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// count the numbers of initial cover and N_{1} sets. Our initial approach to choose cover and N_{1} sets is following:
// j \in N_{1} when x[j] = 1 and a[j] <= 0  and
// j \in C	   when x[j] > 0 and a[j] > 0.
// Note we only consider N_{1} and C, because N_{0} = N - N_{1} - C.

		for (int j=0; j<N; j++) {

			if ((x[j] >= 1 - 1e-06)&&(cutinfo->a[i][j] <= 0)) {
				init_N_1[j] = 1;
				num_Init_N_1++;													
			}
			if ((x[j] >= 1e-06)&&(cutinfo->a[i][j] >= 1e-06)) {
				init_C[j] = 1;
				orderC_ax[num_Init_C] = j;											
				orderC_a[num_Init_C] = j;
				num_Init_C++;
			}
		}
		
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// dealing with the case of initial choice of N_{1} and C satisfies |N_{1}| + |C| < K

		if (num_Init_N_1 + num_Init_C <= K && ( ( !weak && cntct1 <= nmbct1-1 ) || ( weak && cntsp1 <= nmbsp1-1 ) ) ) {
		
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Since |N_{1}| + |C| < K, we should insert more variables into N_{1} \cup C to get real N_{1} and C. ( Note that we need to improve N_{1} and C, but, without bring confusion, we keep using
// the same notation for all different N_{1} and C.) We claim:
// 1. we only add new variables to N_{1};
// 2. the variables added into N_{1} satisfy (assuming the index is j) a_{j} = 0, x_{j} = 0. ( We will have the same result if we choose varibles which satisfy a_{j} = 0 and x_{j} > 0)

// As we can see, sine a_{j} = 0 for all variables that were added into, this lambda calculation is correct.

			for (int j=0; j< N; j++) {
				if ((init_N_1[j]==1)||(init_C[j]==1)) {
					lambda += cutinfo->a[i][j];
				}
			}			
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			if (lambda<=0) goto TERMINATE;
			for (int j=0; j<N; j++) {

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// we get the real N_{1} and C. We check all index in the inital cover set, choose a "good" a_{C}. 
// different a_{C} will give us a different cutting plane.
				
				if ((init_C[j]==1)&&(cutinfo->a[i][j] >= 2*lambda)) {
					for (int l=0; l<N; l++) {														// array N_1 and cover record the real N_{1} and C, we initiate them here
						N_1[l] = init_N_1[l];
						cover[l] = 0;
					}
					a_C = cutinfo->a[i][j];																		// we choose an a_C
					delta = a_C - lambda;
					cover[j] = 1;																	// when we have chosen a_C, of course we have an index in the cover
				} else continue;																	// if we cant have \Delta >= \lambda, we choose another a_{C}
// ??? all of possible a_{C}, should we only choose the smallest a_{C} ???				

				rhs = cutinfo->b[i] + (K - num_Init_N_1 - num_Init_C)*delta;													// this delta here because we insert variables with a_{j} = 0

				for (int l=0; l<N; l++) {
					if ((cutinfo->a[i][l]>=a_C)&&(init_C[l]==1)) {
						cover[l] = 1;																// get the real cover set
					}
					if ((cutinfo->a[i][l]<a_C)&&(init_C[l]==1)) {
						N_1[l] = 1;																	// some variables in the initial cover set should be in real N_{1} now
					}
				}
				
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// check the violation

				cutvio = 0.0;
				for (int l=0; l<N; l++) {
					if (cover[l] == 1) {
						cutval[l] = cutinfo->a[i][l];
						cutvio += cutval[l]*x[l];
					}
					if (N_1[l]==1) {
						rhs -= cutinfo->a[i][l];
						if (cutinfo->a[i][l] <=0) cutval[l] = delta;
						if (cutinfo->a[i][l] > 0) {
							if (a_C > delta + cutinfo->a[i][l]) cutval[l] = delta + cutinfo->a[i][l];
							else cutval[l] = (a_C > cutinfo->a[i][l]) ? a_C : cutinfo->a[i][l];
						}
						rhs += cutval[l];
						cutvio += cutval[l]*x[l];
					}
					if ((x[l]>1e-06)&&(N_1[l]==0)&&(cover[l]==0)) {
						if (cutinfo->a[i][l] >=0) cutval[l] = delta;
						else cutval[l] = (-lambda > cutinfo->a[i][l]) ? delta - lambda : delta + cutinfo->a[i][l];
						cutvio += cutval[l]*x[l];
					}
				}
				cutvio -= rhs;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// get the inequality
				if (cutvio > 1.0) 
				{
					if( weak )
					{
						for (int l=0; l<N; l++) 
							{
								if ((x[l]<=1e-06)&&(N_1[l]==0)&&(cover[l]==0)) 
								{
									if ( cutinfo->a[i][l]>= EPS ) 
										cutval[l] = delta;
									else
									{
										if( cutinfo->a[i][l] <= - EPS ) 
											cutval[l] = (-lambda > cutinfo->a[i][l]) ? delta - lambda : delta + cutinfo->a[i][l];
										else{
											cutval[l] = 0;
										}
									}
								}
								cutind[l] = cutinfo->rel[l];
							}
					}
					else
					{
						if( y_rpl_x )
						{
							for (int l=0; l<N; l++) 
							{
								if ((x[l]<=1e-06)&&(N_1[l]==0)&&(cover[l]==0)) 
								{
									if (cutinfo->a[i][l]>=0) 
									{
										cutval[l] = delta;
										if( cutinfo->a[i][l] <= EPS )
											cutind[l] = cutinfo->clm_y[l]; 
										else
											cutind[l] = cutinfo->rel[l];
									}
									else 
									{
										cutval[l] = (-lambda > cutinfo->a[i][l]) ? delta - lambda : delta + cutinfo->a[i][l];
										cutind[l] = cutinfo->rel[l]; 
									}
								}
								else
									cutind[l] = cutinfo->rel[l];
							}
						}
						else
						{
							for (int l=0; l<N; l++) 
							{
								if ((x[l]<=1e-06)&&(N_1[l]==0)&&(cover[l]==0)) 
								{
									if (cutinfo->a[i][l]>=0) cutval[l] = delta;
									else cutval[l] = (-lambda > cutinfo->a[i][l]) ? delta - lambda : delta + cutinfo->a[i][l];
								}
								cutind[l] = cutinfo->rel[l];
							}
						}
					}
					status = CPXcutcallbackadd(env, cbdata, wherefrom, N, rhs, 'L', cutind, cutval, 0);
					lclct1++;
					if( weak )
						cntsp1++;
					else
						cntct1++;

					if( option == 6 )
					{
						if( ( !weak && cntct1 >= nmbct1 ) || ( weak && cntsp1 >= nmbsp1 ) )
						{
							if( !weak && cntct1 >= nmbct1  )
							{
								printf("*** user cut dense 1 %d \n",cntct1);
//								fprintf(fout, "User cut dense 1 %d at %d \n",cntct1,numnods);
//								fflush(fout);
								last_ct1 = numnods;
							}
							else
							{
								printf("*** user cut sparse 1 %d \n",cntsp1);
//								fprintf(fout, "User cut sparse 1 %d at %d \n",cntsp1,numnods);
//								fflush(fout);
								last_sp1 = numnods;
							}
							goto TERMINATE;
						}
						if( cntct1 >= nmbct1 && cntct2 >= nmbct2 )
							weak = true;
						else
							weak = false;
					}
					else
					{
						if( !weak && cntct1 >= nmbct1 )
						{
							printf("*** user cut dense 1 %d \n",cntct1);
//							fprintf(fout, "User cut dense 1 %d at %d \n",cntct1,numnods);
//							fflush(fout);
							last_ct1 = numnods;
							goto TERMINATE;
						}
						if( weak && cntsp1 >= nmbsp1 )
						{
							printf("*** user cut sparse 1 %d \n",cntct1);
//							fprintf(fout, "User cut sparse 1 %d at %d \n",cntsp1,numnods);
//							fflush(fout);
							last_sp1 = numnods;
							goto TERMINATE;
						}
					}
					if (status) {
						fprintf(stderr, "Failed to add cover inequalities 1. \n");
						goto TERMINATE;
					}
				} // End if( cutvio > 0.1 )
			} // End for j 
		} // End if (num_Init_N_1 + num_Init_C <= K)

	//cin.ignore(); cin.get();
	
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if (num_Init_N_1 + num_Init_C >= K + 1 && ( ( !weak && cntct2 <= nmbct2 - 1 ) || ( weak && cntsp2 <= nmbsp2 - 1 ) ) ) {
			for (int j=0; j < N; j++) {
				if ((x[j] > 1 - 1e-06)&&(cutinfo->a[i][j]>0)) {
					init_N_1[j] = 1;
					init_C[j] = 0;
				}
			}

			for (int j=0; j < N; j++) {
				int size_cnt = num_Init_N_1;
				lambda = -cutinfo->b[i];

				if (init_C[j] == 1) {
					a_C = cutinfo->a[i][j];
				}
				else continue;

				for (int l=0; l<N; l++) {
					cover[l] = 0;
					N_1[l] = init_N_1[l];
				}

				for (int l=0; l<N; l++){
					if ((cutinfo->a[i][l]>=a_C)&&(init_C[l]==1)) {
						size_cnt++;
						cover[l] = 1;
					}
				}

				if (size_cnt > K) {
					for (int l=0; l<N; l++) {
						if ((N_1[l]==1)&&(cutinfo->a[i][l]==0)) {
							N_1[l]=0;
							size_cnt--;
							if (size_cnt==K) break;
						}
					}
				}

				if (size_cnt != K) continue;

				if (size_cnt == K) {
					for (int l=0; l<N; l++) {
						if ((cover[l]==1)||(N_1[l]==1)) {
							lambda += cutinfo->a[i][l];
						}
					}
				}

				if ((a_C >= 2*lambda)&&(lambda>0)) {
					delta = a_C - lambda;
					cutvio = 0.0;
					rhs = cutinfo->b[i]; 
					for (int l=0; l<N; l++) {
						if (cover[l] == 1) {
							cutval[l] = cutinfo->a[i][l];
							cutvio += cutval[l]*x[l];
						}
						if (N_1[l]==1) {
							rhs -= cutinfo->a[i][l];
							if (cutinfo->a[i][l] <=0) cutval[l] = delta;
							if (cutinfo->a[i][l] > 0) {
								if (a_C > delta + cutinfo->a[i][l]) cutval[l] = delta + cutinfo->a[i][l];
								else cutval[l] = (a_C > cutinfo->a[i][l]) ? a_C : cutinfo->a[i][l];
							}
							rhs += cutval[l];
							cutvio += cutval[l]*x[l];
						}
						if ((x[l]>1e-06)&&(N_1[l]==0)&&(cover[l]==0)) {
							if (cutinfo->a[i][l] >=0) cutval[l] = delta;
							else cutval[l] = (-lambda > cutinfo->a[i][l]) ? delta - lambda : delta + cutinfo->a[i][l];
							cutvio += cutval[l]*x[l];
						}
					}
					cutvio -= rhs;

					if (cutvio > 0.01) {
						if( weak )
						{
							for (int l=0; l<N; l++) 
								{
									if ((x[l]<=1e-06)&&(N_1[l]==0)&&(cover[l]==0)) 
									{
										if ( cutinfo->a[i][l]>= EPS ) 
											cutval[l] = delta;
										else
										{
											if( cutinfo->a[i][l] <= - EPS ) 
												cutval[l] = (-lambda > cutinfo->a[i][l]) ? delta - lambda : delta + cutinfo->a[i][l];
											else{
												cutval[l] = 0;
											}
										}
									}
									cutind[l] = cutinfo->rel[l];
								}
						}
						else
						{
							if( y_rpl_x )
							{
								for (int l=0; l<N; l++) 
								{
									if ((x[l]<=1e-06)&&(N_1[l]==0)&&(cover[l]==0)) 
									{
										if (cutinfo->a[i][l]>=0) 
										{
											cutval[l] = delta;
											if( cutinfo->a[i][l] <= EPS )
												cutind[l] = cutinfo->clm_y[l]; 
											else
												cutind[l] = cutinfo->rel[l];
										}
										else 
										{
											cutval[l] = (-lambda > cutinfo->a[i][l]) ? delta - lambda : delta + cutinfo->a[i][l];
											cutind[l] = cutinfo->rel[l]; 
										}
									}
									else
										cutind[l] = cutinfo->rel[l];
								}
							}
							else
							{
								for (int l=0; l<N; l++) 
								{
									if ((x[l] < 1e-06)&&(N_1[l]==0)&&(cover[l]==0)) {
										if (cutinfo->a[i][l]>=0) cutval[l] = delta;
										else cutval[l] = (-lambda > cutinfo->a[i][l]) ? delta - lambda : delta + cutinfo->a[i][l];
									}
									cutind[l] = cutinfo->rel[l];
								}								
							}
						}
						status = CPXcutcallbackadd(env, cbdata, wherefrom, N, rhs, 'L', cutind, cutval, 0);
						lclct2++;
						if( weak )
							cntsp2++;
						else
							cntct2++;

						if( option == 6 )
						{
							if( cntct2 >= nmbct2 && cntsp2 >= nmbsp2 )
							{
								if( cntct2 >= nmbct2 )
									last_ct2 = numnods;
								else
									last_sp2 = numnods;
								goto TERMINATE;
							}
							if( cntct1 >= nmbct1 &&  cntct2 >= nmbct2 )
								weak = true;
							else
								weak = false;
						}
						else
						{
							if( !weak && cntct2 >= nmbct2 )
							{
								last_ct2 = numnods;
								goto TERMINATE;
							}
							if( weak && cntsp2 >= nmbsp2 )
							{
								last_sp2 = numnods;
								goto TERMINATE;
							}
						}
						if (status) {
							fprintf(stderr, "Failed to add cover inequalities 2. \n");
							goto TERMINATE;
						}

/*						for (int l=0; l<N; l++) {
							if ((x[l] < 1e-06)&&(N_1[l]==0)&&(cover[l]==0)) {
								if (cutinfo->a[i][l]>=0) cutval[l] = delta;
								else cutval[l] = (-lambda > cutinfo->a[i][l]) ? delta - lambda : delta + cutinfo->a[i][l];
							}
							cutind[l] = cutinfo->rel[l];
						}

						status = CPXcutcallbackadd(env, cbdata, wherefrom, N, rhs, 'L', cutind, cutval);
						lclct2++;
						if( weak )
							cntsp2++;
						else
							cntct2++;
						if( cntct2 >= nmbct2 )
							goto TERMINATE;

						if (status) {
							fprintf(stderr, "Failed to add cover inequalities. \n");
							goto TERMINATE;
						}
*/					}
				}
			} // End for j

		} // End if (num_Init_N_1 + num_Init_C > K)

	} // End for i (each row)

	*useraction_p = CPX_CALLBACK_SET;

TERMINATE:

	free_and_null ((char **) &x);
	free_and_null ((char **) &y);
	return (status);
} /* END mycutcallback */

int main (){
	int i;
	int num_prb = 0;
	//int myCount = 0;

	int* int_N;
	int* int_M;
	int* int_K;

	char** str_prb;

	printf("Number of problem: ");
	scanf("%d",&num_prb);

	str_prb = new char*[num_prb+1];
	int_N = new int[num_prb+1];
	int_M = new int[num_prb+1];
	int_K = new int[num_prb+1];

	str_prb[0] = new char[100];

	for( i = 1; i <= num_prb ; i++ )
	{
		str_prb[i] = new char[100];
		printf("Enter the problem[%d]: ", i);
		scanf ("%s",str_prb[i]);  
		printf("Enter N[%d]: ", i);
		scanf("%d",&int_N[i]);
		printf("Enter M[%d]: ", i);
		scanf("%d",&int_M[i]);
		printf("Enter D[%d]: ", i);
		scanf("%d", &D);
		printf("Enter K[%d]: ", i);
		scanf("%d",&int_K[i]);
		printf("Enter Count: ");
		scanf("%d", &myCount);
	}

//	set_calldata();

	for( i = 1; i <= num_prb ; i++ )
	{
		if( i == 1 || ( (i >= 2) && (int_N[i-1] != int_N[i-2] || int_M[i-1] != int_M[i-2] || int_K[i-1] != int_K[i-2] ) ) )
		{
			if( i >= 2 )
				del_calldata();
			N = int_N[i];
			M = int_M[i];
			K = int_K[i];
			set_calldata();
		}
		curr_run = 0;
		strncpy (str_prb[1],str_prb[i],100);
		
		printf("The problem is %s\n", str_prb[1]);

		time_t rawtime;
		time ( &rawtime );
		  
		char* str1 = ctime (&rawtime);
		char str2[30];
		char str3[3];
		char str4[3];
		char str5[3];
		char strd[4];
		char strn[3];
		strncpy (str2,str1,20);
		str2[19]='\0';
		strncpy (strd,str2,4);
		strd[3]='\0';
		strncpy (strn,str2+8,4);
		strn[2]='\0';
		strncpy (str2,str1+11,20);
		str2[19]='\0';
		strncpy (str3,str2,2);
		str3[2]='\0';
		strncpy (str4,str2+3,2);
		str4[2]='\0';
		strncpy (str5,str2+6,2);
		str5[2]='\0';

//		char out_name[25];
//		strncpy(out_name,"out_",4);
//		strncpy(&out_name[4],strd,3);
//		strncpy(&out_name[7],"_",1);
//		strncpy(&out_name[8],strn,2);
//		strncpy(&out_name[10],"_",1);
//		strncpy(&out_name[11],str3,2);
//		strncpy(&out_name[13],"_",1);
//		strncpy(&out_name[14],str4,2);
//		strncpy(&out_name[16],"_",1);
//		strncpy(&out_name[17],str5,2);
//		strncpy(&out_name[19],".xls",4);
//		out_name[23] = '\0';

		string out_name, str_N, str_M, str_D, str_myCount;
		stringstream convert;
		convert << setfill('0') << setw(4) << N;
		str_N = convert.str();
		convert.str("");
		convert << setfill('0') << setw(4) << M;
		str_M = convert.str();
		convert.str("");
		convert << setfill('0') << setw(4) << D;
		str_D = convert.str();
		convert.str("");
		convert << setfill('0') << setw(2) << myCount;
		str_myCount = convert.str();
		convert.str("");

		out_name.append(".//Results//");		
		out_name.append(str_N);
		out_name.append("_");
		out_name.append(str_M);
		out_name.append("_");
		out_name.append(str_D);
		out_name.append("_");
		out_name.append(str_myCount);
		out_name.append(".xls");			
		
		FILE *fxls;

//		char str[80]; 
//		strcpy(str, ".//Results//");
//		strcat(str, out_name);
		
		fxls = fopen (out_name.c_str(), "wt"); 
//		fxls = fopen (str, "wt"); 

		nmbcll = 0;
		nmbct1 = 0;
		nmbct2 = 0;
		nmbsp1 = 0;
		nmbsp2 = 0;
		weak = false;
		y_rpl_x = false;
		
		main_1(2,str_prb);
		
		fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
		fflush ( fxls );	

		if( !ONE_TEST )
		{			
			/* strong cuts in x variables */

			nmbcll = 200000000;
			nmbct1 = 1;

			main_1(2,str_prb);	

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );

			nmbct1 = 2;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
		
			nmbct1 = 3;
			
			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			nmbct1 = 4;
			
			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			nmbct1 = 5;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			nmbct1 = 10;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );

			nmbct1 = 20;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			nmbct1 = 30;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			/* strong cuts in y variables */

			if( DENSE_ST )
			{
				nmbct1 = 1;
				weak = false;
				y_rpl_x = true;

				main_1(2,str_prb);	

				fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
				fflush ( fxls );

				nmbct1 = 2;

				main_1(2,str_prb);

				fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
				fflush ( fxls );	
				
				nmbct1 = 3;

				main_1(2,str_prb);

				fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
				fflush ( fxls );			
				
				nmbct1 = 4;

				main_1(2,str_prb);

				fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
				fflush ( fxls );
				
				nmbct1 = 5;

				main_1(2,str_prb);

				fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
				fflush ( fxls );
				
				nmbct1 = 10;

				main_1(2,str_prb);

				fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
				fflush ( fxls );
				
				nmbct1 = 20;

				main_1(2,str_prb);

				fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
				fflush ( fxls );
				
				nmbct1 = 30;

				main_1(2,str_prb);

				fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbct1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
				fflush ( fxls );
			}

			/* weak (sparse) cuts */

			nmbct1 = 0;
			nmbsp1 = 1;
			weak = true;
			y_rpl_x = false;

			main_1(2,str_prb);	

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbsp1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );

			nmbsp1 = 2;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbsp1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			nmbsp1 = 3;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbsp1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			nmbsp1 = 4;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbsp1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			nmbsp1 = 5;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbsp1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			nmbsp1 = 10;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbsp1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			nmbsp1 = 20;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbsp1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
			
			nmbsp1 = 30;

			main_1(2,str_prb);

			fprintf( fxls, "%d	%d	%d	%d	%d	%d	%f	%d	%d	%d	%f	%d \n", myCount, N, M, D, nmbsp1, num_nodes,num_secnd,num_flowc,num_gomoc,num_userc,val_soltn,solstat);
			fflush ( fxls );
		}
	}

	if( i >= 2 )
		del_calldata();

	delete[] int_N;
	delete[] int_M;
	delete[] int_K;

return 0;
}