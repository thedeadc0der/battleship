/*
 * users.c - server-side user management.
 * 
 * The part of the server that handles the list of registered users.
 */

#include "server.h"

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NAME_MAX 64
#define PASS_MAX NAME_MAX
#define PASS_MIN 5


// ============================================================================
// User* functions
// 
// Helper functions to manage the list of registered users.
// ============================================================================
struct SRV_User {
	SRV_User *prev, *next;
	// SRV_Game *current_game;
	char name[NAME_MAX];
	char pass[NAME_MAX];
};

typedef bool(*UserPredicate)(const SRV_User*, void*);

static SRV_User *g_userList;

static SRV_User * UserAdd(const char *name, const char *pass){
	assert(name != NULL && *name != 0);
	assert(pass != NULL && *pass != 0);
	
	// Create the new user
	SRV_User *newUser = (SRV_User*) malloc(sizeof(*newUser));
	
	if( newUser == NULL )
		return NULL;
	
	// Fill it in
	memset(newUser, 0, sizeof(*newUser));
	strcpy(newUser->name, name);
	strcpy(newUser->pass, pass);
	
	// Insert it at the front of the list
	if( g_userList != NULL ){
		newUser->next = g_userList;
		g_userList = newUser;
	} else {
		g_userList = newUser;
	}
	
	return newUser;
}

static SRV_User * UserFind(UserPredicate predicate, void *param){
	assert(predicate != NULL);
	
	for(SRV_User *curr=g_userList; curr != NULL; curr = curr->next){
		if( predicate(curr, param) )
			return curr;
	}
	
	return NULL;
}

#if 0
static void UserDelete(SRV_User *user){
	assert(user != NULL);
	
	// Re-link the previous element.
	if( user->prev != NULL )
		user->prev->next = user->next;
	else
		g_userList = user->next;
	
	// Re-link the next element.
	if( user->next != NULL )
		user->next->prev = user->prev;
	
	// De-allocate it
	free(user);
}
#endif


// ============================================================================
// Registration
// 
// Handles the addition of a new user to the list, with all the necessary checks.
// ============================================================================
static bool ValidateUserName(const char *name){
	size_t len = strlen(name);
	
	if( len > NAME_MAX )
		return false;
	
	for(size_t i=0; i < len; ++i){
		char curr = name[i];
		
		if( !isalnum(curr) && (curr != ' ') && (curr != '-') && (curr != '_') )
			return false;
	}
	
	return true;
}

static bool ValidatePassword(const char *pass){
	size_t len = strlen(pass);
	
	if( (len < PASS_MIN) || (len > PASS_MAX) )
		return false;
	
	for(size_t i=0; i < len; ++i){
		char curr = pass[i];
		
		if( !isalnum(curr) && (curr != ' ') && (curr != '-') && (curr != '_') )
			return false;
	}
	
	return true;
}

SRV_User *SRV_RegisterUser(const char *name, const char *pass, SRV_User_Result *r){
	assert(name != NULL && *name != 0);
	assert(pass != NULL && *pass != 0);
	assert(r != NULL);
	
	// Validate the name and password
	if( !ValidateUserName(name) ){
		*r = SRV_AddUser_BadName;
		return NULL;
	}
	
	if( !ValidatePassword(name) ){
		*r = SRV_AddUser_BadName;
		return NULL;
	}
	
	// Make sure the name is available
	SRV_User *existing = SRV_FindUserByName(name);
	
	if( existing != NULL ){
		*r = SRV_AddUser_NameTaken;
		return NULL;
	}
	
	// Everything's checked, we can add the user.
	return UserAdd(name, pass);
}


// ============================================================================
// Retrieval
// 
// Retrieves users based on various criteria
// ============================================================================
static bool FindByNamePredicate(const SRV_User *user, const char *name){
	return strcmp(user->name, name) == 0;
}

typedef struct {
	const char *name;
	const char *pass;
} NameAndPass;

static bool FindByNameAndPassPredicate(const SRV_User *user, const NameAndPass *p){
	return (strcmp(user->name, p->name) == 0) && (strcmp(user->pass, p->pass) == 0);
}

SRV_User *SRV_FindUserByName(const char *name){
	assert(name != NULL);
	
	// These aren't possible, so save time.
	if( *name == 0 )
		return NULL;
	
	return UserFind((UserPredicate) FindByNamePredicate, (void*) name);
}

SRV_User *SRV_FindUserByNameAndPass(const char *name, const char *pass){
	assert(name != NULL);
	assert(pass != NULL);
	
	// These aren't possible, so save time.
	if( *name == 0 || *pass == 0 )
		return NULL;
	
	NameAndPass p = {
		.name = name,
		.pass = pass,
	};
	
	return UserFind((UserPredicate) FindByNameAndPassPredicate, &p);
}


// ============================================================================
// Encapsulation
// ============================================================================
const SRV_User *SRV_UserList(){
	return g_userList;
}

const SRV_User *SRV_NextUser(const SRV_User *u){
	assert(u != NULL);
	return u->next;
}

const char * SRV_UserName(const SRV_User *user){
	assert(user != NULL);
	return user->name;
}


// ============================================================================
// Saving/Loading of the users file
// ============================================================================
static const char USERS_FILE[] = "users.csv";

void SRV_SaveUsers(){
	FILE *fp = fopen(USERS_FILE, "wb");
	
	if( fp == NULL ){
		perror("Could not save user list");
		return;
	}
	
	for(SRV_User *curr=g_userList; curr != NULL; curr = curr->next)
		fprintf(fp, "%s;%s;\n", curr->name, curr->pass);
	
	fflush(fp);
	fclose(fp);
}

void SRV_LoadUsers(){
	// If we can't load the users file, we must be on an initial run.
	FILE *fp = fopen(USERS_FILE, "rb");
	
	if( fp == NULL ){
		perror("Could not load user list");
		return;
	}
	
	
	
	fclose(fp);
}


