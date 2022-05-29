//program to define the class server
#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>
#include <sys/ipc.h>
#include <fstream>
#include <sstream>
#include "../include/server.h"
#define ADDRSERV "127.0.0.1"
#define MAX 256
#define PORT 8028
using namespace std;


Server::Server(){
                        
                           
                       
                };
//loading thefile containing username,password and group informations to the vector

int Server::loadDAta(){
            fstream inputfile;
            string line;
            inputfile.open("/home/cguser11/phonebook_management/db/authentication.txt");
            if(inputfile.is_open()){											//checking whether the file is opened
            	while(getline(inputfile,line)){
                  	stringstream ss(line);
                        string username;
                        string password;
                        string group;
                        vector<string> groups;
                        getline(ss,username,':');
                        getline(ss,password,':');
                        while(getline(ss,group,':')){
                        	groups.push_back(group);								//pushing the group information into a vector
                        }
                        if(username!="" && password != "" ){
                        	users.push_back(User(username,password,groups));					//pushing the object into the user vector
                        }
                  }
	      inputfile.close();											//closing the file
            }
            else{
            	cout << "No file existing " << endl ;
            }
	    return 0;

}

//function socket creation

int Server::createSocket(){
            slen=sizeof(sockaddr_in);
            memset(&servaddr,0,slen);
            memset(&cliaddr,0,slen);

            servaddr.sin_family = AF_INET; //To use Internet addressing family
            servaddr.sin_addr.s_addr = inet_addr(ADDRSERV);
            servaddr.sin_port = htons(PORT);


            sockfd=socket(AF_INET, SOCK_STREAM, 0);                                         				//creating the socket
            if(sockfd < 0){
      	      perror("Socket creation has not been done");
            	//exit(EXIT_FAILURE);
            }
	    return 0;
}

//function to bind the socket
int Server::bindSocket(){
            int ret;
            ret=bind(sockfd, (struct sockaddr *)&servaddr, slen);  							//binding the socket
            if(ret < 0){
	            fputs("Binding has not been done",stderr);
            //    exit(EXIT_FAILURE);
             }
	     return 0;
}

//function to call listening to the port 8028
int Server::listenTo(){
            int ret;
            ret=listen(sockfd, 5);											//listening to the port 8028
            if(ret< 0){
	            fputs("Socket is not Listening",stderr);
      	      exit(EXIT_FAILURE);
            }
	    return 0;
}

//function to acceptconncetion and and concurrency by using child process
 void Server::acceptConnections(){

	while(1){
		connectfd=accept(sockfd,(struct sockaddr*)&servaddr,(socklen_t *)&slen);
            if(connectfd < 0){
            	fputs("Conncetion is not established",stderr);
                  //      exit(EXIT_FAILURE);
            }
            pid = fork();												//creating the child process
            if (pid == 0){
	    		string filename = "" ;
              		string type =authenticateUser();								//calling the authenticate function to check the username is authentic user
                             string type1 ;
                             if(type != "admin" && type != ""){	
                             	type1 = "authenticated user" ;
                              }
                              else if(type ==""){
                              	type1 ="anonymous user" ;
                               }
                                else if(type == "admin"){
                               	type1="admin";
                                }
                                mlen=send(connectfd,type1.c_str(),strlen(type1.c_str()),0);				//sending the type of user to the client
                                while(1){
                                	string recvdata = recvData();							//function to receive data 
                                            if(type1=="authenticated user"){
                                           		  string command ,input1 ,input2;
                                                            stringstream ss(recvdata);
                                                            getline(ss,command,' ');
                                                            if(command == "ADD"){					//checking if the command is ADD

                                                            	getline(ss,input1,',');
                                                                        getline(ss,input2,',');
                                                                        User user;
                                                                        string ret = user.addData(input1,input2,filename);//calling the adddata function of user class	
                                                                        if(ret != "no"){
                                                                        		char positive[]="Contact added" ;
                                                                                       send(connectfd,positive,strlen(positive),0);	//sending the message to the client
                                                                      }
                                                                      else{
                                                                    	  	char negative[]="The user can't add the data";
                                                                        		send(connectfd,negative,strlen(negative),0);
                                                                     }
                                                      }
                                                      else if(command == "chgrp"){							//checking if the command is chgrp
                                                      		ss >> input1 ;
                                                     		for(auto user :users){
                                                                        		if(user.findUser(type)){			//calling the find user function using vector of User object
                                                                                		filename=user.chgrp(input1);		//calling the change group functionof user class
                                                                               		 break;
                                                                        		}
                                                                	}
                                                                	if(filename =="no"){
                                                                        		char neg[]="The user doesnt belong to the group";
                                                                        		send(connectfd,neg,strlen(neg),0);
                                                                	}
                                                                	else{
                                                                        		char pos[]="Group changed" ;
                                                                        		send(connectfd,pos,strlen(pos),0);
                                                                	}


                                                        	}
                                                        else if(command == "list"){							//checking if the command is list
                                                                ss >> input1 ;
                                                                User user ;
                                                                vector<string> contacts;
                                                                contacts = user.listData(input1,filename);
                                                                string concat = "";
                                                                if(!contacts.empty()){
                                                                        for(auto contact : contacts ){

                                                                                concat += contact ;
                                                                        }
                                                                        send(connectfd,concat.c_str(),concat.size(),0);
                                                                }
                                                                else{
                                                                        char buffer[]="File is empty ";
                                                                        send(connectfd,buffer,strlen(buffer),0);
                                                                }

                                                        }
                                                        else if(command == "rm"){							//checking if the command is remove
                                                                ss >> input1;
                                                                string msg = "" ;
                                                                User user;
                                                                msg = user.removeContact(input1,filename);				//calling the user function to remove the contact
                                                                if(msg=="yes"){
                                                                        char posi[] = "Contact removed " ;
                                                                        send(connectfd,posi,strlen(posi),0);
                                                                }
                                                                else{
                                                                        char nega[]="Entered user is not present";
                                                                        send(connectfd,nega,strlen(nega),0);

                                                                }
                                                        }

                                                }
                                                if(type1=="anonymous user"){								//checking whether the user id anonymous
                                                        string command ,input1 ,input2;
                                                        stringstream ss(recvdata);
                                                        getline(ss,command,' ');
                                                        if(command == "ADD"){								//checking if the command is ADD
                                                                filename="public group";		
                                                                getline(ss,input1,',');
                                                                getline(ss,input2,',');
                                                                User user;
                                                                user.addData(input1,input2,filename);
                                                                char positive[]="Contact added" ;
                                                                send(connectfd,positive,strlen(positive),0);
                                                        }

                                                }


                                        }
                                }
                                signal(SIGCHLD,SIG_IGN);										//signal to avoid child process
                        }
                }

//function to receive data
string Server::recvData(){
	char recvdata[1024] ;
        memset(recvdata,0,1024);
        recv(connectfd , recvdata,1024 , 0) ;
        if(strcmp(recvdata,"bye")==0 || strcmp(recvdata,"Bye")==0){									//checking for bye if bye then client closes
        	close(connectfd);
                exit(0);
        }
        return string(recvdata) ;
}
//function to authenticate user
string Server::authenticateUser(){
	char  receivedata[1024];
        memset(receivedata,0,1024);
        string username ,password ;
        recv(connectfd ,receivedata,1024 , 0) ;
        string recvdata=string(receivedata);
        stringstream ss(recvdata);
        getline(ss ,username , ':');
        getline(ss , password, ':');
        for (auto i : users){
        	if(i.authenticate(User(username , password))){
                	return username;
                }
        }
        return "";
}





