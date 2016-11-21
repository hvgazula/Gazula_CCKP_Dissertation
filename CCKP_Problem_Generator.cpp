/*
File Name	: CCKP_Problem_Generator.cpp
Author		: Harshvardhan Gazula
Modified on : 07/23/2016
Changes made: 1) Added a loop for variable 'p' --> Number of instances for each N-M-D combination
			  2) Put the function srand() outside p loop so different instances are generated else
			  	 same instances are being generated
*/

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>

using namespace std;

int* ord;

int setNum = 1;

int c_min =  1; // 1;
int c_max =  25;

int a_min = -20;
int a_max =  20;

int n_prb = 1;

double nk = (double) 0.2;
double pp = (double) 0.8;
//double nm = (double) 0.1; // i added
float nm = (float) 0.05;

clock_t T;

bool in_vector( int*& v, int dim, int elem )
{
	for( int i = 0; i < dim; i++)
	if( v[i] == elem )
	return true;

	return false;
}

static int compare(const void *a, const void *b)
{
	return (ord[*(int *) a] < ord[*(int *) b]) ? - 1 : 1;
} 

int* quick_sort(int*& v,int dim)
{
	if( dim == 0 || v == NULL)
	return NULL;

	ord = v;

	int* r = new int[dim];

	for ( int j = 0; j < dim; j++ ) 
	r[j] = j;

	qsort( r, dim, sizeof(int), compare );

	ord = NULL;

	return r;
}

int main()
{
	int i;
	int j;
	int p;
	int q;

	int m = 0;
	int n = 0;
	int k = 0;
	float d = 0;

	int nz;
	int nz_p;
	int nz_n;
	int* c;
	int* b;
	int** a_v;
	int** a_i;
	int* o;

	int* smm;
	int* mxm;

	string etichetta_seed;
	string etichetta_n;
	string etichetta_m;
	string etichetta_d;
	string etichetta_p;
	string str_setNum;
	stringstream convert;
	
	if( c_min >= c_max + 1 || a_min >= a_max + 1 )
	{
		cout << "Inconsisten parameters error" << endl;
		return 0;
	}

	ofstream file;
	ofstream log;

// 	string log_name;
//	log_name.append("Problem_Set_");
//	convert << setfill('0') << setw(2) << setNum;
//	str_setNum = convert.str();		
//	convert.str("");
//	log_name.append(str_setNum);
//	log_name.append(".log");
//	log.open(log_name.c_str());

	srand(time (NULL));
		
	for (n = 1000; n <= 1000; n += 1000)
	{			
/* 		
		log_name.append("Problem_Set_");
		convert << setfill('0') << setw(4) << n;
		str_setNum = convert.str();		
		convert.str("");
		log_name.append(str_setNum);
		log_name.append(".log");
		log.open(log_name.c_str()); 
*/
	
		for (nm = 0.25; nm <= 1.01; nm += 0.25)
		{					
			
			for (d = 25; d <= 100; d += 25)
			{	
				
				for (p = 1; p <= 1; p += 1)
				{		
					
					m = (int) ( nm * n );
					k = (int) ( nk * n );
					
					smm = new int[m];
					mxm = new int[m];

					nz = (int) ( 0.01 * n * d );

					if( a_min >= 0 && a_max >= 0 )
					{
						nz_p = nz;
						nz_n = 0;		}
					else
					{
						if( a_min <= -1 && a_max <= -1 )
						{
							nz_p = 0;
							nz_n = nz;
						}
						else
						{
							nz_p = (int) ( nz * pp );
							nz_n = nz - nz_p;
						}
					}

					c = new int[n];
					b = new int[m];
					a_v = new int*[m];
					a_i = new int*[m];

					for( i = 0; i < m; i++ )
					{
						a_v[i] = new int[nz];
						a_i[i] = new int[nz];
					}
					
					convert << setfill('0') << setw(4) << n;
					etichetta_n = convert.str();
					convert.str("");
					convert << setfill('0') << setw(4) << m;
					etichetta_m = convert.str();
					convert.str("");
					convert << setfill('0') << setw(4) << d*n/100;
					etichetta_d = convert.str();
					convert.str("");
					convert << setfill('0') << setw(2) << p;
					etichetta_p = convert.str();
					convert.str("");

/* 					
					log << "n = " << n << endl;
					log << "m = " << m << endl;
					log << "k = " << k << endl;
					log << "density = " << d << endl;
					log << "positive = " << pp << endl; 
*/
					
					string file_name;

					// profit coefficient generation
					for( i = 0 ; i < n ; i++ ){
						c[i] = c_min + ( rand() % ( c_max - c_min ) ); 
					}				

					// knapsack coefficient generation
					for( i = 0 ; i < m ; i++ )
					{
						smm[i] = 0;
						mxm[i] = - a_min;

						if( pp <= - 1 )
						{
							for( j = 0 ; j < nz ; j++ )
							{
								a_v[i][j] = a_min + ( rand() % ( a_max - a_min ) ); 
								q = rand() % n;
								while( in_vector( a_i[i], j, q ) ) 
								{
									q = rand() % n;
								}

								a_i[i][j] = q; // rand() % n; 

								if( a_v[i][j] >= 1 )
								smm[i] = smm[i] + a_v[i][j] ; // abs( a_v[i][j] );
								if( mxm[i] <= a_v[i][j] - 1 ) // abs( a_v[i][j] ) + 1 )
								mxm[i] = a_v[i][j] ; // abs( a_v[i][j] );
							}
						}
						else
						{
							for( j = 0 ; j < nz_p ; j++ )
							{
								a_v[i][j] = max( a_min, 1 ) + ( rand() % ( a_max - max( a_min, 0 ) ) ); 
								q = rand() % n;
								while( in_vector( a_i[i], j, q ) ) 
								{
									q = rand() % n;
								}

								a_i[i][j] = q;  

								if( a_v[i][j] >= 1 )
								smm[i] = smm[i] + a_v[i][j] ; 
								if( mxm[i] <= a_v[i][j] - 1 ) 
								mxm[i] = a_v[i][j] ; 
							}
							for( j = nz_p ; j < nz ; j++ )
							{
								a_v[i][j] = a_min + ( rand() % ( min( a_max, 0 ) - a_min ) ); 
								q = rand() % n;
								while( in_vector( a_i[i], j, q ) ) 
								{
									q = rand() % n;
								}

								a_i[i][j] = q;  

								//						smm[i] = smm[i] + a_v[i][j] ; 
								//						if( mxm[i] >= a_v[i][j] + 1 ) 
								//						 	mxm[i] = a_v[i][j] ; 
							}
						}
					}

					// right hand side generation			
					for( i = 0 ; i < m ; i++ ){
						b[i] = max( (int)( 0.1 * smm[i] ), 1 + mxm[i] );
					}
					
					// problem generation
					file_name.append(etichetta_n);
					file_name.append("_");
					file_name.append(etichetta_m);
					file_name.append("_");
					file_name.append(etichetta_d);
					file_name.append("_");
					file_name.append(etichetta_p);
					file_name.append(".lp");

					// log << "file = " << file_name.c_str() << endl << endl;

					// cout << "Generating File - " << file_name.c_str() << endl;
					
					file.open(file_name.c_str());
					
					file << "\\n = " << n << endl;
					file << "\\m = " << 100*nm << "% of " << n << endl;
					file << "\\k = " << 100*nk << "% of " << n << endl;
					file << "\\d = " << d << "% of " << n << endl;
					file << "\\np = " << 100*pp << "% of non-zero coefficients" << endl;
					
					file << "maximize" << endl;
					i = 0;
					while( true )
					{
						if( abs( c[i] ) >= 1 )
						{
							file << c[i] << " x" << i << " ";
							break;
						}

						i++;
					}
					for( j = i+1 ; j < n ; j++ )
					{
						if( abs( c[j] ) >= 1 )
						{
							if( c[j] >= 1 )
							{
								file << "+ ";
							}
							else
							{
								file << "- ";
							}
							if( abs( c[j] ) >= 2 )
							file << abs( c[j] );
							file << " x" << j << " ";
						}
					}
					file << endl;
					file << "Subject to" << endl;
					for( i = 0 ; i < m ; i++ )
					{
						file << "c" << i << ": ";

						o = quick_sort( a_i[i], nz );

						for( j = 0 ; j < nz ; j++ )
						{
							if( abs( a_v[i][o[j]] ) >= 1 )
							{
								if( a_v[i][o[j]] >= 1 )
								{
									if( j >= 1 )
									file << "+ ";
								}
								else
								file << "- ";
								if( abs( a_v[i][o[j]] ) >= 2 )
								file << abs( a_v[i][o[j]] ) << " ";
								file << "x" << a_i[i][o[j]] << " ";
							}
						}
						file << "<= " << b[i] << endl;

						delete[] o;
					}
					for( i = 0 ; i < n ; i++ )
					{
						file << "ub" << i << ": x" << i << " - y" << i << " <= 0" << endl;
					}

					file << "K : y0 ";
					for( i = 1; i < n ; i++ )
					file << " + y" << i;
					file << " <= " << k << endl;
					file << "Bound" << endl;
					for( i = 0; i < n ; i++ )
					file << "y" << i << " <= 1" << endl;
					file << "Binaries" << endl;
					for( i = 0; i < n ; i++ )
					{
						file << "y" << i << " ";
						if( (i+1) % 5 == 0 || i == n-1 )
						file << endl;
					}
					file << "End";
					file.close();				
					

					
					delete[] smm;
					delete[] mxm;
					delete[] c;
					delete[] b;
					for( i = 0; i < m; i++ )
					{
						delete[] a_v[i];
						delete[] a_i[i];
					}
					delete[] a_v;
					delete[] a_i;
					
				} // end of d loop
			} // end of p loop			
		}// end of m loop		
	} // end of n loop
	
	// log.close();
} // end of main function