/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSide.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaizpuru <jaizpuru@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 10:00:28 by jaizpuru          #+#    #+#             */
/*   Updated: 2024/03/19 16:55:34 by jaizpuru         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/server/ClientSide.hpp"

// Librerias Iker
#include <filesystem>
#include <iostream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <string>

ClientSide::ClientSide( void ) {}

ClientSide::ClientSide( int _serverFd ) {
	timeout.tv_sec = 5; // 5 seconds for Client
	timeout.tv_usec = 0;
	bzero(clientMsg, sizeof(clientMsg));
	if ((clientFd = accept(_serverFd, (sockaddr *)&clientAddr, &addrClientLen)) < 0)
	throw std::logic_error("error: accept");

	if (setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
		throw(std::runtime_error("error: setsockopt()"));

	//! Recieve message
	returnedBytes = recv(clientFd, clientMsg, sizeof(clientMsg), 0);
	if (returnedBytes < 0) {
		close(_serverFd);
		close(clientFd);
		throw std::logic_error("recv error");
	} else if (returnedBytes == 0) { // Connection closed by the client
		close(_serverFd);
		close(clientFd);
		throw std::runtime_error("client closed connection");
	} else // Data received, process it
		std::cout << "Bytes received: " << returnedBytes << std::endl;

	openFile(getRoute());
	std::cout << std::endl << "AQUI --> " << clientMsg << std::endl;

	//! Response
	/* serverResponse = "HTTP/1.1 200  OK\r\n\r\n <html><head></head><body><h1 text-family=\"Roboto\" align=\"center\"> Hello, Inception42! </h1></body></html>";
		std::string msgRet(clientMsg);
	std::cout << "EYY -> " << msgRet << std::endl;
	if (msgRet.find("favicon.ico", 0) != std::string::npos) {
		msgRet = readFaviconFile("resources/favicon.ico");
		std::string httpResponse = "HTTP/1.1 200 OK\r\n";
		httpResponse += "Content-Type: image/x-icon\r\n";
		httpResponse += "Content-Length: " + std::to_string(msgRet.size()) + "\r\n";
		httpResponse += "\r\n";
		httpResponse += msgRet;
		send(clientFd, httpResponse.data(), httpResponse.size(), 0);
	}
	else if (msgRet.find("web.html", 0) != std::string::npos) {
		msgRet = readFaviconFile("resources/web.html");
		std::string httpResponse = "HTTP/1.1 200 OK\r\n";
		httpResponse += "Content-Type: text/html\r\n";
		httpResponse += "Content-Length: " + std::to_string(msgRet.size()) + "\r\n";
		httpResponse += "\r\n";
		httpResponse += msgRet;
		send(clientFd, httpResponse.data(), httpResponse.size(), 0);
	}
	else
	send(clientFd, serverResponse.data(), serverResponse.size(), 0);
	close(clientFd);
	*/
	response2client();
	 // After server has replied, close connection
}

ClientSide::~ClientSide( void ) {}

std::string	ClientSide::getRoute( void ) {
	std::string str(clientMsg);
	std::stringstream ss;

	size_t start = str.find("/", 0);
	if (str[start] == '/' && str[start + 1] == ' ')
		;
	else {
		for (start = start + 1; str[start] != ' '; start++) {
			ss << str[start];
		}
	}
	if (ss.str().empty()) // abort open, since user did not ask for any file
		ss << "none";
	std::cout << "Route : " << ss.str() << std::endl;
	std::string ret(ss.str());
	return (ret);
}

int ClientSide::openFile( std::string _route ) {
	if (!_route.compare("none"))
		return 1;
	std::ifstream file;
	file.open(_route.c_str());
	if (file.is_open() > 0 )
		;
	else { // return 404 page
		_route = "resources/404.html";
		file.open(_route.c_str());
	}	
	char ret[1028];
	while (file.getline(ret, 1028)) {
		std::cout << "Size to read : " << file.gcount() << " : ";
		std::cout << ret << std::endl;
	}
	std::cout << ret << std::endl;
	file.close();
	return 0;
}

bool isDirectory(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
}

std::string generate_autoindex(const std::string& directoryPath, string autoindex) {

    // Abre el directorio
    DIR* dir = opendir(directoryPath.c_str());
    if (!dir) {
        return autoindex; // Error al abrir el directorio
    }

    autoindex += "<html><head><title>Index of " + directoryPath + "</title></head>\n";
    autoindex += "<body><h1>Index of " + directoryPath + "</h1><hr><ul>\n";

    // Lee el contenido del directorio
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        autoindex += "<li><a href=\"" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a></li>\n";
    }

    autoindex += "</ul><hr></body></html>\n";

    // Cierra el directorio
    closedir(dir);

    return autoindex;
}

bool isAbsolutePath(const std::string& path) {
    // Verifica si la ruta comienza con "http://" o "https://"
    return (path.find("http://") == 0 || path.find("https://") == 0);
}

void ClientSide::getMethod( void )
{
	std::map<std::string, std::string> location;
	//std::map<std::string, std::string> alias;
	unsigned long max_length = 3000;
	location["/web2.html"] = "web.html";
	location["/web3.html"] = "http://localhost:405/extra/web3.html";
	location["/Extra/hola.html"] = "https://www.youtube.com";
	//alias["/kapouet"] = "/mi_carpeta";
	std::string allowed_methods = "GET POST";
	std::cout << "GETMETHOD" << std::endl;
	std::string httpResponse;
	if (allowed_methods.find("GET") == std::string::npos)
	{
		std::string httpResponse = "HTTP/1.1 405 Method Not Allowed\r\n";
    	httpResponse += "Allow: " + allowed_methods + "\r\n";
    	httpResponse += "\r\n";
		std::cout << "Error allowed methods" << std::endl;
		send(clientFd, httpResponse.data(), httpResponse.size(), 0);
	}
	else
	{
		if (location.find(file) != location.end())
		{
			if (isAbsolutePath(location[file]))
			{
    			// Redirección 301
				std::cout << "AQUI VA" << std::endl;
    			httpResponse = "HTTP/1.1 302 Found\r\n";
    			httpResponse += "Location: " + location[file] + "\r\n";
				httpResponse += "\r\n";
				send(clientFd, httpResponse.data(), httpResponse.size(), 0);
			}
			else if (!location[file].empty())
			{
				httpResponse = "HTTP/1.1 301 Moved Permanently\r\n";
				std::string host = "http://localhost:405/";
    			httpResponse += "Location: " + host + location[file] + "\r\n";
    			httpResponse += "\r\n";
    			send(clientFd, httpResponse.data(), httpResponse.size(), 0);
			}
			else
			{
				httpResponse = "HTTP/1.1 500 Internal Server Error\r\n";
				send(clientFd, httpResponse.data(), httpResponse.size(), 0);
			}
	    }
		else
		{
			std::ifstream archivo("resources/GET" + file);
			std::cout << "Hola estoy aqui " << "resources/GET" + file << std::endl;
			std::ostringstream oss;
			std::string directoryPath = "resources/GET" + file;
			if (isDirectory(directoryPath)) {
                // Generar autoindex
                std::string autoindex;
                autoindex = generate_autoindex(directoryPath, autoindex);
                // Respuesta 200 OK con el autoindex
                httpResponse = "HTTP/1.1 200 OK\r\n";
                httpResponse += "Content-Type: text/html\r\n";
                httpResponse += "Content-Length: " + std::to_string(autoindex.size()) + "\r\n";
                httpResponse += "\r\n";
                httpResponse += autoindex;
                send(clientFd, httpResponse.data(), httpResponse.size(), 0);
            } else if (archivo.is_open()){
				std::cout << "y" << std::endl;
				oss << archivo.rdbuf();
				std::string httpResponse;
				if (oss.str().size() > max_length)
				{
					httpResponse = "HTTP/1.1 413 Request Entity Too Large\r\n";
					httpResponse += "\r\n";
					send(clientFd, httpResponse.data(), httpResponse.size(), 0);
				}
				else
				{
					// Respuesta 200 OK
	    			httpResponse = "HTTP/1.1 200 OK\r\n";
					httpResponse += "Content-Type: text/html\r\n";
					httpResponse += "Content-Length: " + std::to_string(oss.str().size()) + "\r\n";
					httpResponse += "\r\n";
					httpResponse += oss.str();
					send(clientFd, httpResponse.data(), httpResponse.size(), 0);
				}
	    	}
			else
			{
				std::ifstream archivo("resources/GET/404.html");
	    		oss << archivo.rdbuf();
				std::string httpResponse = "HTTP/1.1 404 Not Found\r\n";
				httpResponse += "Content-Type: text/html\r\n";
				httpResponse += "Content-Length: " + std::to_string(oss.str().size()) + "\r\n";
				httpResponse += "\r\n";
				httpResponse += oss.str();
				send(clientFd, httpResponse.data(), httpResponse.size(), 0);
			}
		}
	}
	
	std::cout << "TERMINO" << std::endl;
	close(clientFd);
}

void ClientSide::postMethod( void )
{
	std::cout << "ENTRO AL METODO POST" << std::endl;
	std::string httpResponse;
	std::string allowed_methods = "GET POST"; // Métodos permitidos
    if (allowed_methods.find("POST") == std::string::npos) {
        httpResponse = "HTTP/1.1 405 Method Not Allowed\r\n";
        httpResponse += "Allow: " + allowed_methods + "\r\n";
        httpResponse += "\r\n";
        std::cout << "Error: Método no permitido" << std::endl;
        send(clientFd, httpResponse.data(), httpResponse.size(), 0);
        return;
    } else {
		std::string msgString(clientMsg);
	    size_t bodyStart = msgString.find("\r\n\r\n");
		std::string postBody = msgString.substr(bodyStart + 4);
	    if (postBody.empty()) {
	        std::cout << "HTTP/1.1 204 No Content\r\n" << std::endl;
	        httpResponse = "HTTP/1.1 204 No Content\r\n";
	        httpResponse += "\r\n";
	        send(clientFd, httpResponse.data(), httpResponse.size(), 0);
	        return;
	    }
	    std::cout << "Body: " << postBody << std::endl;
		std::ifstream verificarArchivo(("resources/GET/" + file).c_str());
    	bool archivoExiste = verificarArchivo.good();
    	verificarArchivo.close();

	    std::ofstream archivo(("resources/GET/" + file).c_str(), std::ios::app);
	    if (archivo.is_open()) {
        	if (archivoExiste) {
            	archivo << postBody << std::endl;
            	std::cout << "Contenido escrito exitosamente al final del archivo." << std::endl;
            	std::cout << "HTTP/1.1 200 OK\r\n" << std::endl;
        	} else {
            	archivo << postBody << std::endl;
            	std::cout << "Contenido escrito exitosamente en el archivo nuevo." << std::endl;
            	std::cout << "HTTP/1.1 201 Created\r\n" << std::endl;
        	}
	    } else {
	        std::cerr << "HTTP/1.1 500 Internal Server Error\n" << std::endl;
	        httpResponse = "HTTP/1.1 500 Internal Server Error\r\n";
	        httpResponse += "\r\n";
	        send(clientFd, httpResponse.data(), httpResponse.size(), 0);
	    }
	}
}

void ClientSide::deleteMethod( void )
{
	std::cout << "DELETEMETHOD" << std::endl;
	std::cout << "Hola estoy aqui " << "resources/GET" + file << std::endl;
	std::string httpResponse = "HTTP/1.1 500 Internal Server Error\r\n";
    // Intentar eliminar el archivo
    if (std::remove(("resources/GET" + file).c_str()) != 0)
	{
		if (isDirectory("resources/GET" + file))
		{
			std::string httpResponse = "HTTP/1.1 409 Conflict\r\n";
    		httpResponse += "Content-Type: text/plain\r\n";
    		httpResponse += "\r\n";
    		httpResponse += "No se puede eliminar el directorio. Elimine los archivos dentro del directorio primero.\r\n";
			std::cout << "J" << std::endl;
		}
		else{
			httpResponse = "HTTP/1.1 404 Not Found\r\n";
			std::cout << "Y" << std::endl;
		}
	}
	else
	{
		httpResponse = "HTTP/1.1 200 OK\r\n";
		std::cout << "O" << std::endl;
	}
	send(clientFd, httpResponse.data(), httpResponse.size(), 0);
	std::cout << "TERMINO" << std::endl;
	close(clientFd);
}

std::string replaceAlias(const std::string& file, const std::map<std::string, std::string>& alias) {
    std::string result = file;
    
    for (std::map<std::string, std::string>::const_iterator it = alias.begin(); it != alias.end(); ++it) {
        const std::string& aliasPath = it->first;
        const std::string& replacement = it->second;
        
        size_t pos = result.find(aliasPath);
        
        // Si se encuentra el alias, reemplazarlo
        while (pos != std::string::npos) {
            result.replace(pos, aliasPath.length(), replacement);
            pos = result.find(aliasPath, pos + replacement.length());
        }
    }
    std::cout << "result = " << result << std::endl;
    return result;
}

void ClientSide::response2client( void )
{
	std::istringstream ss(clientMsg);
	std::map<std::string, std::string> alias;
    alias["micuenta"] = "Extra";
	
	ss >> method >> file;
	file = replaceAlias(file, alias);
	if (method == "GET")
		getMethod();
	else if (method == "POST")
		postMethod();
	else if (method == "DELETE")
		deleteMethod();
}


// Tema alias
// Si tengo un alias /kapouet = /ssr/iker y me hacen la peticion --> /kapouet/mi_carpeta/web5.html --> yo buscar el archivo en /ssr/iker/mi_carpeta/web5.html
