#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define size 4

int main(void){

  	int i,brojDivljaka=10,brObroka;
    
	key_t k1=15;
    key_t k2=25;
    int shmid1, shmid2, *shm1,*shm2,*s1 ,*s2, x1,x2;
    pid_t pid;

    x2=0; //Broj nahranjenih

    printf("Unesite broj obroka po loncu:\t");
    scanf("%d", &brObroka);
    
    x1=brObroka;
    
    shmid1=shmget(k1, size,IPC_CREAT|0760); //Pravljenje deljene memorije za broj nahranjenih i broj obroka
    shmid2=shmget(k2,size,IPC_CREAT|0760);
    
    for (i = 1; i <= brojDivljaka; i++) {
        shm1=(int *)shmat(shmid1,0,0);
		shm2=(int *)shmat(shmid2,0,0);
		s1=shm1;
		s2=shm2;
		*s1=x1;
		*s2=x2;

        pid = fork(); //pocetak forka
        if (pid == 0) {
	    printf("Divljak broj %d je ogladneo, vreme je za jelo!\n", i);
	    printf("U loncu je trenutno %d obroka!\n", *shm1);

	    (*shm1)--;
	    (*shm2)++;

	    x1=*shm1;
	    x2=*shm2;
        printf("Divljak broj %d je pojeo svoj obrok\n\n\n", i);
	   
	    if(*shm2==10) {
	      printf("Divljaci su konacno nahranjeni i kuvar je otisao na spavanje!\n");
	      return 0;
	    }
	   
	    if(*shm1==0) {
	      printf("Divljak %d je pojeo poslednji obrok iz lonca i otisao je da budi kuvara!\n", i);
	      printf("Kuvara je probudio divljak\n");
	      printf("Kuvar priprema %d obroka\n", brObroka);
	      *shm1=brObroka;
	    }

	    shmdt(shm1);
	    shmdt(shm2);
	    exit(0);
	}
	wait(NULL);
	x1=*s1;
	x2=*s2;
    }


    return 0;
}