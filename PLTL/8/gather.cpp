//note:
//"ready" is a named shared array as big as par_count which needs to be initialized with zeros.
//Probably wait a second before forking the kids, so proc 0 can zero the "ready" array

void synch(int par_id, int par_count, int* ready)
	{

	int synchid = ready[par_count] + 1;
	ready[par_id] = synchid;
	int breakout = 0;
	while (1)
		{

		breakout = 1;
		for (int i = 0; i < par_count; i++)
			{

			if (ready[i] < synchid)
				{
				breakout = 0;
				break;
				}
			}
		if (breakout == 1)
			{
			ready[par_count] = synchid;
			break;
			}
		}
	}



