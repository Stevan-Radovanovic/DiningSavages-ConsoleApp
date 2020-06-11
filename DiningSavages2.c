#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int brObroka = 0; 
int brObrokaZaSpremanje = 0;	

void kuvar (int signal) {
	if(signal==0) {
        printf("Kuvara je probudio divljak\n");
        printf("Kuvar priprema %d obroka\n", brObrokaZaSpremanje);
        brObroka=brObrokaZaSpremanje;
        printf("Kuvar je pripremio %d obroka\n", brObroka);
        } else {
            printf("Divljaci su konacno nahranjeni. Kuvar moze da se spakuje, i da konacno ode kuci!\n");
        }
}


int main(void){
    int i;
    int brojDivljaka = 10;
    pid_t pid;
    int brNahranjenih=0;

    printf("Unesite broj obroka po loncu:\t");
    scanf("%d", &brObroka);

    brObrokaZaSpremanje = brObroka; // [Pamtimo broj obroka u pomocnu promenljivu, zato sto se broj obroka menja, a kuvar mora napraviti isi broj kada se probudi]

    for (i = 1; i <= brojDivljaka; i++) { // [For petlja za broj divljaka koji treba da jedu]
        pid = fork();
        if (pid == 0) {
            printf("Divljak broj %d je ogladneo, vreme je za jelo!\n", i);
            printf("Divljak broj %d je pojeo svoj obrok\n", i);
	    exit(0);
	}
	pid=wait(NULL);
	brObroka--;
    brNahranjenih++;
    if(brNahranjenih==10) {kuvar(1); return 0;}
    if(brObroka==0) {
        printf("Divljak %d je pojeo poslednji obrok iz lonca i otisao je da budi kuvara!\n", i);
        kuvar(0);
        }
    }
    return 0;
}