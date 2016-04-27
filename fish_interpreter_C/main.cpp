#include "fish.h"
#include <ctime>

void main()
{
	fish f;
	while (f.get_script())
	{
		clock_t begin = clock();
		f.read_script();
		f.display();
		f.run();
		f.show_output();
		clock_t end = clock();
		long double elapsed_secs = long double(end - begin) / (long double) CLOCKS_PER_SEC;
		cout << "\n Elapsed time : " << elapsed_secs << " seconds\n";
	}
}