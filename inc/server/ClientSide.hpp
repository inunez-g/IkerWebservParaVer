/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSide.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaizpuru <jaizpuru@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 09:54:41 by jaizpuru          #+#    #+#             */
/*   Updated: 2024/03/19 16:30:45 by jaizpuru         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Utils.hpp"

class ClientSide {
	private:
    /* Client Server Side */
    int							clientFd;
    struct sockaddr_in			clientAddr; // IP socket address
    socklen_t					addrClientLen;

	/* Client Message Side */
    int							returnedBytes;
    char						clientMsg[1028];
	std::string					serverResponse; // Header for Client
    struct  timeval				timeout;

	  // Variables Iker
    std::string method, file;

	public:
		ClientSide( void );
		ClientSide( int _serverFd );
		~ClientSide( void );

		int 	openFile( std::string _route );
		std::string	getRoute( void );

		// Funciones Iker
        void    getMethod( void );
        void    postMethod( void );
        void    deleteMethod( void );
        void    response2client( void );

};