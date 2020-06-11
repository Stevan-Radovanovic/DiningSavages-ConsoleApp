#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>



void *divljak(void* ID);
void *kuvar();

char ime1[] = "Obroci";
char ime2[] = "Krevet kuvara";

sem_t* obroci;	// [Ovaj semafor oznacava broj preostalih obroka]
sem_t* krevetKuvara; // [Semafor inicijalno postavljen na nulu, sluzi za budjenje kuvara]

int brObroka = 0; // [Isti kao vrednost semafora "obroci"]
int brObrokaZaSpremanje = 0;	// [Promenljiva koja oznacava koliko obroka treba da napravi kuvar]
				

int signal = 0;			// [Indikator da li je program gotov]

int main(void)
{
	
	pthread_t kuvarNit;
	pthread_t divljakNit[20]; // [Maksimalan broj divljaka postavljamo na 20]
	

	int i;					// [Iterator]
	int brojDivljaka;		// [Promenljiva koju unosimo]
	int nizDivljaka[20];	// [Pomocni niz koji se koristi pri prosledjivanju parametara funkciji "divljak", sluzi da za svakog divljaka napravimo nit]
	
	printf("Unesite broj obroka po loncu:\t");
	scanf("%d", &brObroka);

	brObrokaZaSpremanje = brObroka;	// [Pamtimo broj obroka u pomocnu promenljivu, zato sto se broj obroka menja, a kuvar mora napraviti isi broj kada se probudi]
	
	UNOS: printf("Unesite broj divljaka: ");
		  scanf("%d", &brojDivljaka);

	if(brojDivljaka > 20)
	{
		printf("Najveci broj divljaka je 20!\n");
		goto UNOS;
	}

	obroci = sem_open(ime1,O_CREAT,0644,0); // [Imenujemo dva semafora koja smo napravili]
	krevetKuvara = sem_open(ime2,O_CREAT,0644,0);


	sem_init(obroci, 0, brObroka);		// [Inicijalizacija semafora "obroci"]
	sem_init(krevetKuvara, 0, 0);		// [Inicijalizacija semafora "krevetKuvara"]

	pthread_create(&kuvarNit, NULL, kuvar, NULL);	// [Pravimo novu nit koja simulira rad kuvara]

	for(i = 0; i < brojDivljaka; i++)		
		nizDivljaka[i] = i;				// [Pravi se pomocni niz za ID parametre u funkciji "void *divljak(void* ID);"]

	for(i = 0; i < brojDivljaka; i++)
		pthread_create(&divljakNit[i], NULL, divljak, (void*)&nizDivljaka[i]); 	// [Prave se niti koje simuliraju divljake]

	for(i = 0; i < brojDivljaka; i++) {
		pthread_join(divljakNit[i], NULL);	// [Sve niti se spajaju]
	}									

	signal = 1;					// [Indikator da su sve niti sa divljacima izvrsene, tj sve su se "sacekale" i "spojile"]

	sem_post(krevetKuvara);			// [Budimo kuvara, zato sto je posao gotov]

	pthread_join(kuvarNit, NULL);			// [Nit kuvara se spaja]

	sem_close(obroci);				// [Zatvaranje i unlinkovanje nasa dva semafora]
	sem_unlink(ime1);
	sem_destroy(obroci);
	sem_close(krevetKuvara);
	sem_unlink(ime2);
	sem_destroy(krevetKuvara);


	return 0;
}


void *divljak(void *ID)
{
	sleep(rand()%10);
	int div = *(int *) ID;		// [Konverzija radi lakseg pisanja]

	printf("Divljak broj %d je ogladneo, vreme je za jelo!\n", div+1);	// [Nit odredjenog divljaka se izvrsava] 

	sem_wait(obroci);		// [V-operacija nad semaforom]
	brObroka--;			// [Smanjujemo "brObroka" koji prate smanjenje semafora "obroci"]

	printf("Divljak broj %d je pojeo svoj obrok\n", div+1);	// [Indikator da je divljak pojeo svoj obrok]
	
	if(brObroka == 0)	
	{
		/* [Ako je broj obroka jednak nuli, to znaci da je onaj divljak koji je poslednji uzeo obrok, ostavio za sobom prazan
		lonac. To je znak da treba buditi kuvara da pravi jos!] */

		printf("Divljak %d je pojeo poslednji obrok iz lonca i otisao je da budi kuvara!\n", div+1);

		

		sem_post(krevetKuvara);	// [Operacija P nad semaforom]
	}
}

void *kuvar()
{
	while(!signal)
	{
		sleep(rand()%10);
		printf("Kuvar spava\n");
		sem_wait(krevetKuvara); 	// [Uspavamo kuvara dok se ne izvrsi : "sem_post(krevetKuvara)"]

		printf("Kuvara je probudio divljak\n");		
			
		if(!signal) 	// [Sve dok i poslednji divljak ne pojede svoj obrok]
		{
			printf("Kuvar priprema %d obroka\n", brObrokaZaSpremanje);
			
			for(int i = 0; i < brObrokaZaSpremanje; i++)
			{
				sem_post(obroci);	// [Izvrsava se P-operacija nad semaforom "obroci"]
				brObroka++;		// [Vrednost promenljive "brObroka" prati vrednost semafora "obroci"]
				
			}	
				
			printf("Kuvar je pripremio %d obroka\n", brObrokaZaSpremanje);	// [Obavestenje da je kuvar spremio obroke]
		}
	
		else
			printf("Divljaci su konacno nahranjeni. Kuvar moze da se spakuje, i da konacno ode kuci!\n");
	}
}
