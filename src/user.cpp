//program to define the class user
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>
#include <sys/ipc.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <user.h>

using namespace std;


//default constructor        
User::User(){};



//parameterized constructor to load username and password
User::User::User(string username ,string password){
	this->username = username;
        this->password = password;
}



//parametrized constructor to load the username and password with group information
User::User(string username ,string password ,vector<string>group){
	this->username = username;
        this->password = password;
        for(auto i: group){
      	  this->group.push_back(i);
        }
}




//function to authenticate user return bool

bool  User::ToAuthenticate(User user){
	if(this->username == user.username && this->password == user.password ){
      	return true ;
      }
      else{
      	return false ;
      }
}



//function to add data to the phonebook 
string User::ToAddData(string input1 ,string input2,string filename){
      Phonebook phonebook(input1,input2);
      if(phonebook.ToStoreData(filename)){
      	return "yes";
      }
      else{
      	return "no";
      }

}


//function to add data to the phonebook 
string User::ToAdminAddData(string input1 ,string input2,string filename){
      ifstream infile;
      infile.open("/home/cguser11/phonebook_management/db/" + filename + ".txt");
      Phonebook phonebook(input1,input2);
      if(infile.is_open()){
      	infile.close();
      	if(phonebook.ToStoreData(filename)){
      		return "yes";
      	}
      }
      return "no";

}



//function to remove contact from the phonebook
string User::ToRemoveContact(string input,string filename){
      Phonebook phonebook ;
      vector <string> afterRemoved ;
      string line = "" ;
      ifstream infile;
      int count1 = 0 ;
      int count2 = 0 ;
      infile.open("/home/cguser11/phonebook_management/db/"+filename + ".txt");
      if(infile.is_open()){
      	while(getline(infile,line)){
            	  string name ,phonenumber ;
                  stringstream ss(line);
                  if(getline(ss,name,',')){
		  	count1++;
		  }
                  getline(ss,phonenumber,',');
                  if(name!="" && name!=input){											//checking the username to exclude the username and store in a vector
            	      afterRemoved.push_back(name + "," + phonenumber );
		      count2++ ;
                  }			
              }
         }
         infile.close();
	 if(count1==count2){
	 	return filename ;
	 }
         string yes =phonebook.ToRemoveData(afterRemoved,filename);								//calling the removedata function to remove the data
         return yes ;
}




//function to list the data with the substring and returns a vector oof string
vector<string> User::ToListData(string input,string filename){
      ifstream infile ;
      vector<string> listedContact;
      string line = "";
      if(filename!=""){

            infile.open("/home/cguser11/phonebook_management/db/"+filename + ".txt");
            listedContact.push_back("The contacts in the " + filename + " group\n");
            if(infile.is_open()){
            	while(getline(infile,line)){
                  	string name ,phonenumber ;
                        stringstream ss(line);
                        getline(ss,name,',');
                        getline(ss,phonenumber,',');
                        if(name != "" && phonenumber !=""){
                        	if(name.substr(0,input.size()) == input){
                              	listedContact.push_back("Name :" + name + "\t\tPhonenumber :" + phonenumber + "\n");
                              }
                        }
            	}
     		}
    		infile.close();
     	}
      else{
      	infile.open("/home/cguser11/phonebook_management/db/public group.txt");
            if(infile.is_open()){
            	listedContact.push_back("The contacts in the public group\n");
                  while(getline(infile,line)){
                  	string name ,phonenumber ;
                        stringstream ss(line);
                        getline(ss,name,',');
                        getline(ss,phonenumber,',');
                        if(name != "" && phonenumber != ""){
                        	if(name.substr(0,input.size()) == input){
                        	      	listedContact.push_back("Name :" + name + "\t\tPhonenumber :" + phonenumber + "\n");
                              }
                        }
                  }
             }
     infile.close();
     }
     return listedContact;
}





//function to find user return bool
bool User::ToFindUser(string username){
	return this->username == username ;
}






//function to change the group
string User::ToChgrp(string input){
	for(auto grpName : group){
      		if(grpName == input){
        	    	return input ;
        	}
       }
       return "no";

}



//function to add group to the directory
int User::ToAddGrp(string filename){
	ofstream outfile ;
	outfile.open("/home/cguser11/phonebook_management/db/"+filename+".txt");
	if(outfile.is_open()){
		return 1;
	}
	outfile.close();
	return 0;
}



//function to remove group from the directory
int User::ToRemoveGrp(string filename){
	filename = "/home/cguser11/phonebook_management/db/" + filename +".txt";
	ifstream infile;
	infile.open(filename);
	if(infile.is_open()){
		remove(filename.c_str());
		return 1;
	}
	return 0 ;
}

