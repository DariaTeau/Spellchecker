#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>

// List element for Dictionary lists.
typedef struct ListNode {
	struct ListNode *next;
	struct ListNode *prev;
	char *key;
	char *value;
	int frequency;
} ListNode;

// Dictionary structure that includes the lists of elements and their number.
typedef struct Dictionary {
	ListNode **lists;		// lists of elements.
	int N;					// number of lists.
} Dictionary;

// Initializes an empty Dictionary structure.
Dictionary* createDictionary(int N);

// Adds an element to the Dictionary structure.
void addElement(Dictionary *dictionary, char *key, char *value, int frequency);

// Removes an element from the Dictionary structure.
void removeElement(Dictionary *dictionary, char *key, char *value);

// Prints all the elements from all the lists of the Dictionary structure.
void printDictionary(FILE *f, Dictionary *dictionary);

// Gets all the elements with the specified key and increments the frequencies.
ListNode* get(Dictionary *dictionary, char *key);

// Prints all the elements with the specified value.
void printValue(FILE *f, Dictionary *dictionary , char *value);

// Prints all the elements with the specified frequency.
void printFrequency(FILE *f, Dictionary *dictionary , int frequency);

// Returns a list containing the elements with the specified value.
ListNode* unionValues(Dictionary *dictionary, char *value);

// Returns a list containing the elements with maximum frequency in each list.
ListNode* unionMaxFrequencies(Dictionary *dictionary);

// Returns a new Dictionary with reversed lists of the input structure.
Dictionary* reverseLists(Dictionary *dictionary);

// Prints a double-linked non-circular list.
void printList(FILE *f, ListNode *list);

// Frees all the memory allocated for the Dictionary.
void freeDictionary(Dictionary *dictionary);

// Frees all the memory allocated for a double-linked non-circular list.
void freeList(ListNode *list);

//------------------------------------------------------------------------------
int nr_elemente(ListNode *list);

//creeaza o lista dublu inlantuita circulara vida
ListNode* createList(){
	ListNode *list = calloc(1,sizeof(ListNode));
	if(list == NULL)
		return NULL;

	list->key = calloc(30,sizeof(char));

	if(list->key == NULL){
		free(list);
		return NULL;
	}

	list->value = calloc(30,sizeof(char));

	if(list->value == NULL){
		free(list->key);
		free(list);
		return NULL;
	}
	list->next = NULL;
	list->prev = NULL;
	return list;

}
Dictionary* createDictionary(int N) {
	Dictionary *d = (Dictionary*)calloc(1,sizeof(Dictionary));
	if(d ==	NULL)
		return 0;

	d->N = N;
	d->lists = (ListNode**)calloc(N,sizeof(ListNode*));
	if(d->lists == NULL)
		return 0;
	int i;
	for(i = 0;i < N;i++){
		d->lists[i] = createList();

		/*in caz ca nu reuseste alocarea trebuie
		dezalocate si celelalte elemente ale dictionarului*/
			
		if(d->lists[i] == NULL){
			while(--i){
				free(d->lists[i]->value);
				free(d->lists[i]->key);
				free(d->lists[i]);
			}
			free(d->lists);
			return 0;
		}
	}

	return d;
}

//intoarce numarul de elemente dintr-o lista
int nr_elemente(ListNode *list){
	int nr=0;
	ListNode *t;
	t = list;
	if(list->prev != NULL && list->next != NULL && list != NULL){
		do{
			t = t->next;
			nr++;
		}while(t != list);
	}

	return nr;
}

/*intoarce 0 daca elementul nu exista in lista, iar
daca exista intoarce frequency-ul acestuia*/
int exists(ListNode *list,char *key, char *value){
	if(list->next == NULL && list->prev == NULL)
		return 0;
	ListNode *aux=list;
	do{
		if(strcmp(aux->key,key) == 0 && strcmp(aux->value,value) == 0){
			return aux->frequency;
		}

		aux = aux->next;
	}while(aux != list);
	return 0;
}
void addElement(Dictionary *dictionary, char *key, char *value, int frequency) {

	int suma_caractere = 0,i;
	/*calculeaza suma caracterelor lui key
	pentru a determina indicele listei*/
	for(i = 0;i < strlen(key);i++)
		suma_caractere = suma_caractere + key[i];

	int r = suma_caractere % dictionary->N;

	ListNode *new_el = createList();
	if(new_el == NULL)
		return;
	
	int suma = nr_elemente(dictionary->lists[r]);

	/*daca elemntul nu exista, iar numarul elementelor
	este egal cu N sterg ultimul element din lista*/
	if(exists(dictionary->lists[r],key,value) == 0){
		if(suma == dictionary->N){

			removeElement(dictionary,dictionary->lists[r]->prev->key,dictionary->lists[r]->prev->value);
		}
	}

	/*modific frequency pentru elementele deja continute, iar pentru
	cele care nu exista frequency nu se schimba deoarece exists
	intoarce 0*/
	frequency = frequency + exists(dictionary->lists[r],key,value);

	/* daca elementul exista il sterg pentru a-l
	aduga pe cel cu noul frequency*/
	if(exists(dictionary->lists[r],key,value) != 0)
		removeElement(dictionary,key,value);

	strcpy(new_el->key,key);
	strcpy(new_el->value,value);
	new_el->frequency = frequency;
	new_el->next = NULL;
	new_el->prev = NULL;

	//actualizez suma in caz ca am sters vreun element
	suma = nr_elemente(dictionary->lists[r]);
	ListNode *aux = dictionary->lists[r];
	i=0;
	//tratez cazul in care lista nu are niciun element
	if(suma == 0){
		new_el->next = new_el;
		new_el->prev = new_el;
		dictionary->lists[r] = new_el;
		return;

	}
	while(i < suma){
		
		if(aux->frequency < frequency){
			//cazul in care elementul trebuie pus in capul listei
			if(aux == dictionary->lists[r]){
				new_el->next = dictionary->lists[r];
				new_el->prev = dictionary->lists[r]->prev;
				dictionary->lists[r]->prev->next = new_el;
				dictionary->lists[r]->prev = new_el;
				dictionary->lists[r] = new_el;
				break;
			}
			
			new_el->next = aux;
			new_el->prev = aux->prev;
			aux->prev->next = new_el;
			aux->prev = new_el;
			break;
		}
		/*cazul in care ajunge la finalul listei
 		si nu a fost inserat inca, atuci va fi 
 		inserat la final*/
		if(aux == dictionary->lists[r]->prev){
			if(aux->frequency > frequency){
				new_el->next = aux->next;
				new_el->prev = aux;
				aux->next->prev = new_el;
				aux->next = new_el;
				break;
			}
		}
		if(aux->frequency == frequency){

			if(strcmp(aux->value,value) > 0 ){
				//cazul in care elementul trebuie pus in capul listei
				if(aux == dictionary->lists[r]){
					new_el->prev = dictionary->lists[r]->prev;
					new_el->next = dictionary->lists[r];
					dictionary->lists[r]->prev->next = new_el;
					dictionary->lists[r]->prev = new_el;
					dictionary->lists[r] = new_el;
					break;
				}
					
				new_el->prev = aux->prev;
				new_el->next = aux;
				aux->prev->next = new_el;
				aux->prev = new_el;
				break;
			}
			/*cazul in care ajunge la finalul listei
 			si nu a fost inserat inca, atuci va fi 
 			inserat la final*/
			if(aux == dictionary->lists[r]->prev){
				if(strcmp(aux->value,value) < 0){
					new_el->next = aux->next;
					new_el->prev = aux;
					aux->next->prev = new_el;
					aux->next = new_el;
					break;
				}
			}


			if(strcmp(aux->value,value) == 0){
				if(strcmp(aux->key,key) > 0){
					//cazul in care elementul trebuie pus in capul listei
					if(aux == dictionary->lists[r]){
						new_el->prev = dictionary->lists[r]->prev;
						new_el->next = dictionary->lists[r];
						dictionary->lists[r]->prev->next = new_el;
						dictionary->lists[r]->prev = new_el;
						dictionary->lists[r] = new_el;
						break;
					}
				
					new_el->next = aux;
					new_el->prev = aux->prev;
					aux->prev->next = new_el;
					aux->prev = new_el;
					break;
				}
				/*cazul in care ajunge la finalul listei
 				si nu a fost inserat inca, atuci va fi 
 				inserat la final*/
				if(aux == dictionary->lists[r]->prev){
					if(strcmp(aux->key,key) < 0){
						new_el->next = aux->next;
						new_el->prev = aux;
						aux->next->prev = new_el;
						aux->next = new_el;
						break;
					}
				}
			}
		}

		aux = aux->next;
		i++;
	}
	int total = 0;
	//calculez cate tupluri se afla in dictionar
	for(i = 0;i < dictionary->N;i++){
		total = total + nr_elemente(dictionary->lists[i]);
	}
	//daca depaseste 2*N sterg ultimul element al fiecarei liste
	if(total > 2*dictionary->N){
		for(i = 0;i < dictionary->N;i++){
			//cazul in care lista are un singur element, iar dupa stergere devine vida
			if(dictionary->lists[i]->prev == dictionary->lists[i] && dictionary->lists[i]->next == dictionary->lists[i]){
				dictionary->lists[i]->prev = NULL;
				dictionary->lists[i]->next = NULL;
			}else{

				if(dictionary->lists[i]->prev != NULL && dictionary->lists[i]->next != NULL){
					removeElement(dictionary,dictionary->lists[i]->prev->key,dictionary->lists[i]->prev->value);
				}
			}

		}
	}

}

void removeElement(Dictionary *dictionary, char *key, char *value) {

	int suma_caractere=0,i;

	for(i = 0;i < strlen(key);i++)
		suma_caractere = suma_caractere + key[i];

	int r = suma_caractere % dictionary->N;
	//cazul in care lista e vida
	if(dictionary->lists[r]->next == NULL && dictionary->lists[r]->prev == NULL)
		return;

	ListNode *auxiliar = dictionary->lists[r];

	int suma;
	//numarul elementelor din lista
	suma = nr_elemente(dictionary->lists[r]);
	//cazul in care lista are n singur element, iar dupa sergere devine vida
	if(suma == 1){
		if(strcmp(auxiliar->key,key) == 0 && strcmp(auxiliar->value,value) == 0){
			auxiliar->next = NULL;
			auxiliar->prev = NULL;
			auxiliar->key[0] = 0;
			auxiliar->value[0] = 0;
			return;
		}
	}
	i = 0;
	while(i < suma){
		if(strcmp(auxiliar->key,key) == 0 && strcmp(auxiliar->value,value) == 0){
			//cazul in care trebuie sters elementul din capul listei
			if(auxiliar == dictionary->lists[r]){
				dictionary->lists[r]->next->prev = dictionary->lists[r]->prev;
				dictionary->lists[r]->prev->next = dictionary->lists[r]->next;
				dictionary->lists[r] = dictionary->lists[r]->next;
				return;
			}
			auxiliar->prev->next = auxiliar->next;
			auxiliar->next->prev = auxiliar->prev;
			return;
		}
		auxiliar = auxiliar->next;
		i++;
	}
}
void printDictionary(FILE *f, Dictionary *dictionary) {
	int i;
	for(i=0;i<dictionary->N;i++){
		fprintf(f,"List %d: ",i);
		//verific daca lista este vida sau nealocata
		if(dictionary->lists[i]->next != NULL & dictionary->lists[i]->next != NULL){

			ListNode *auxiliar = dictionary->lists[i];

			while(auxiliar->next != dictionary->lists[i]){

				fprintf(f,"(%s, %s, %d) ",auxiliar->key,auxiliar->value,auxiliar->frequency);
				auxiliar = auxiliar->next;
			}
			/*printez o data dupa while deoarece cand auxiliar e pe ultimul element nu
			intra in while*/
			fprintf(f,"(%s, %s, %d) ",auxiliar->key,auxiliar->value,auxiliar->frequency);	
		}
		fprintf(f,"\n");
	}
	
}
//adauga la finalul unei liste necirculare(este implementata mai jos)
void AddEnd(ListNode **list, char *value, char *key, int frequency);

ListNode* get(Dictionary *dictionary, char *key) {
	int i,k=0;
	ListNode *new_el=createList();
	ListNode *list=createList();

	for(i = 0;i < dictionary->N;i++){
		ListNode *aux = dictionary->lists[i];
		ListNode *new_list = createList();

		if(dictionary->lists[i]->next != NULL && dictionary->lists[i]->prev != NULL){
			do{
				if(strcmp(aux->key,key) == 0){
					k++;
					//adaug intr-o lista dublu inlantuita necirculara
					AddEnd(&list,aux->value,aux->key,aux->frequency);
					strcpy(new_el->key,aux->key);
					strcpy(new_el->value,aux->value);
					new_el->frequency = aux->frequency;
					//adaug intr-o lista dublu inlantuita necirculara
					AddEnd(&new_list, new_el->value, new_el->key, new_el->frequency);
				}
				aux = aux->next;

			}while(aux != dictionary->lists[i]);
			/*folosesc a doua lista pentru a creste cu 1 frequency-ul
			in dictionar pentru elemntele gasite*/
			while(new_list != NULL && new_list->frequency !=0){
				addElement(dictionary,new_list->key,new_list->value,1);
				new_list = new_list->next;
			}
		}
	}
	//daca elementul nu exista intoarce 0
	if(k == 0)
		return 0;
	return list;
}

void printValue(FILE *f, Dictionary *dictionary , char *value) {
	int i;
	for(i = 0;i < dictionary->N;i++){

		ListNode *auxiliar = createList();
		auxiliar = dictionary->lists[i];

		if(dictionary->lists[i]->next != NULL & dictionary->lists[i]->next != NULL){	
			do{
				if(strcmp(auxiliar->value,value) == 0)
					fprintf(f,"(%s, %s, %d) ",auxiliar->key,auxiliar->value,auxiliar->frequency);
				auxiliar = auxiliar->next;

			}while(auxiliar!=dictionary->lists[i]);
		}
	}
	fprintf(f,"\n");

}

void printFrequency(FILE *f, Dictionary *dictionary , int frequency) {
	int i;
	for(i = 0;i < dictionary->N;i++){

		ListNode *auxiliar = createList();
		auxiliar = dictionary->lists[i];

		if(dictionary->lists[i]->next != NULL && dictionary->lists[i]->next != NULL){	
			do{
				if(auxiliar->frequency == frequency)
					fprintf(f,"(%s, %s, %d) ",auxiliar->key,auxiliar->value,auxiliar->frequency);
				auxiliar = auxiliar->next;

			}while(auxiliar != dictionary->lists[i]);
		}
	}
	fprintf(f,"\n");	
	
}
//adauga elemente la finalul unei liste necirculare
void AddEnd(ListNode **list, char *value, char *key, int frequency){

	ListNode *new_el = createList();
	strcpy(new_el->key,key);
	strcpy(new_el->value,value);
	new_el->frequency = frequency;

	ListNode *aux = (*list);
	//cazull in care lista e vida
	if((*list)->next == NULL && (*list)->prev == NULL && (*list)->frequency == 0){
		strcpy((*list)->key,key);
		strcpy((*list)->value,value);
		(*list)->frequency = frequency;
		//pastrez prev si next NULL pentru ca lista e necirculara
		(*list)->next = NULL;
		(*list)->prev = NULL;
		return;
	}
	//cazul in care lista are un singur element
	if((*list)->next == NULL && (*list)->prev == NULL && (*list)->frequency != 0){
		new_el->next = NULL;
		(*list)->next = new_el;
		new_el->prev = (*list);
		(*list)->prev = NULL;
		return;
	}
	//cand ies din while aux va fi ultimul element
	while(aux->next != NULL){
		aux=aux->next;
	}
	new_el->next = NULL;
	aux->next = new_el;
	new_el->prev = aux;

}
ListNode* unionValues(Dictionary *dictionary, char *value) {
	int i;
	ListNode *list = createList();

	for(i = 0;i < dictionary->N;i++){

		ListNode *auxiliar = createList();
		auxiliar = dictionary->lists[i];

		if(dictionary->lists[i]->next != NULL && dictionary->lists[i]->next != NULL){	
			do{
				if(strcmp(auxiliar->value,value) == 0)
					//pun elementul intr-o lista dublu inlantuita necirculara
					AddEnd(&list,auxiliar->value,auxiliar->key,auxiliar->frequency);
				auxiliar = auxiliar->next;

			}while(auxiliar != dictionary->lists[i]);
		}
	}
	return list;
	
}

ListNode* unionMaxFrequencies(Dictionary *dictionary) {
	int i;
	ListNode *new_el = createList();
	ListNode *list = createList();

	for(i = 0;i < dictionary->N;i++){
		ListNode *aux = dictionary->lists[i]->next;

		if(dictionary->lists[i]->next != NULL && dictionary->lists[i]->next != NULL){
			//adaug primul element pentru ca el are cel mai mare frequency
			AddEnd(&list,dictionary->lists[i]->value,dictionary->lists[i]->key,dictionary->lists[i]->frequency);
			//testez daca mai exista elemente cu acelasi frequency si le adaug
			while(aux != dictionary->lists[i]){
				if(dictionary->lists[i]->frequency == aux->frequency)
					AddEnd(&list,aux->value,aux->key,aux->frequency);
				aux = aux->next;
			}
		}
	}
	return list;
}
/*adauga elementul la coada unei liste dublu inlantuite
circulare*/
void add(ListNode **l,char *key,char *value,int frequency){
	if(*l == NULL)
		return;

	ListNode *new_el = createList();
	strcpy(new_el->key,key);
	strcpy(new_el->value,value);
	new_el->frequency = frequency;
	new_el->prev = NULL;
	new_el->next = NULL;
	//cazul in care lista e vida
	if((*l)->next == NULL && (*l)->prev == NULL){
		(*l) = new_el;
		(*l)->next = new_el;
		(*l)->prev = new_el;
		return;
	}

	(*l)->prev->next = new_el;
	new_el->prev = (*l)->prev;
	new_el->next = (*l);
	(*l)->prev = new_el;

}
Dictionary* reverseLists(Dictionary *dictionary) {

	Dictionary *d=createDictionary(dictionary->N);
	int i;
	for(i=0;i<dictionary->N;i++){
		//parcurge lista de la final spre inceput
		ListNode *auxiliar = dictionary->lists[i]->prev;
	 	if(dictionary->lists[i] != NULL && dictionary->lists[i]->next != NULL && dictionary->lists[i]->prev != NULL){
	 		do {
	 			//adauga elementul in noul dictionar
	 			add(&(d->lists[i]),auxiliar->key,auxiliar->value,auxiliar->frequency);

	 			auxiliar = auxiliar->prev;
	 		}while(auxiliar != dictionary->lists[i]->prev);
	 	}
	}	
	return d;
}

void printList(FILE *f, ListNode *list) {

	if(list == NULL){
		fprintf(f,"\n");
		return;
	}
	//se parcurge pana la NULL deaoarece este necirculara
	while(list != NULL){
		//daca frequency e 0 atunci celula e vida si nu se afiseaza nimic
		if(list->frequency != 0)
			fprintf(f,"(%s, %s, %d) ",list->key,list->value,list->frequency);
		list = list->next;
	}
	fprintf(f,"\n");
	
}

void freeDictionary(Dictionary *dictionary) {
	int i;
	
	if(dictionary == NULL)
		return;
	for(i = 0;i < dictionary->N;i++){
		ListNode *auxiliar = dictionary->lists[i];
		ListNode *aux;
		
		if(dictionary->lists[i] != NULL){
			/*face ca ultimul element sa pointeze 
			la NULL pentru a face lista ecirculara*/
			if(dictionary->lists[i]->prev != NULL)
				dictionary->lists[i]->prev->next = NULL;
			//parcurge o lista necirculara
			while (auxiliar != NULL) {	
				//in aux pastrez elementul anterior
				aux = auxiliar;
				auxiliar = auxiliar->next;

				free(aux->key);
				free(aux->value);
				free(aux);
			}
		}
	}
	free(dictionary->lists);
	free(dictionary);
}

void freeList(ListNode *list) {
	ListNode *aux = list;
	ListNode *p;
	//parcurge pana la NULL pentru ce e necirculara
	while(aux != NULL){
		//in p pastrez elementul anterior
		p = aux;
		aux = aux->next;
		free(p->key);
		free(p->value);
		free(p);
	}
}

#endif
