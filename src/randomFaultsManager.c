#include "randomFaultsManager.h"
#include "xoroshiro128plus.h"

extern void sample_dip_dipdir(double mean_dip, double mean_dip_dir, double dip_var, double dip_dir_var, double *new_dip, double *new_dip_dir);

void initRandomFaultManager(FaultManager *fltsManager, xrshr128p_state_t state)
{
    int nbFamilies = 1+(rand()%3); //amandine // Select a specific number of fault families. Between 1 and 3
    int nbFaults;

    fltsManager->family_number = (size_t) nbFamilies;
    fltsManager->families = malloc(fltsManager->family_number * sizeof(FaultFamily));

    if (fltsManager->families == NULL) {
        perror("Erreur d'allocation");
        return ;
    }

    int numFaults = 5;

    for (size_t i = 0; i < fltsManager->family_number; i++) {

        FaultFamily *family = &fltsManager->families[i];
        
        family->family_dip = 60.0 + 20.0*(xrshr128p_next_double(&state));
        family->family_dipdir = 360.0 * xrshr128p_next_double(&state); ;
        family->faults_number = 1 + (xrshr128p_next(&state)%numFaults);
        family->faults = malloc(family->faults_number * sizeof(RandomFault));

        if (family->faults == NULL) {
            perror("Erreur d'allocation");
            return ;
    }

        for (size_t j = 0; j < family->faults_number; j++){

            RandomFault *fault = &family->faults[j];

            double conjuguate = xrshr128p_next(&state)%2;

            double dip = 0;
            double dipdir =0;

            sample_dip_dipdir(family->family_dip, family->family_dipdir, 5, 20, &dip, &dipdir);

            dipdir = (dipdir + 180.0*conjuguate);
				if (dipdir>= 360.0)
					dipdir -= 360.0;

            fault->dip = dip;
            fault->dipdir = dipdir;
            fault->slip = 2000.0 * xrshr128p_next_double(&state);
            fault->pitch = 80.0 + 10.0 * xrshr128p_next_double(&state);

            fault->posX = 0.0 + 5000.0 * xrshr128p_next_double(&state);
            fault->posY = 0.0 + 5000.0 * xrshr128p_next_double(&state);
            fault->posZ = 500.0 + 4500.0 * xrshr128p_next_double(&state);

            fault->Xaxis = 0;
            fault->Yaxis = 0; 
            fault->Zaxis = 0;
        }
    }
}

void FreeRandomFaultManager(FaultManager *fltsManager){

    for (size_t i = 0; i < fltsManager->family_number; i++) {

        FaultFamily *family = fltsManager->families[i];

        free(family->faults);
    }

    free(fltsManager->families);

}

