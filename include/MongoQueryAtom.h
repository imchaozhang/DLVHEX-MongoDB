/* 
 * File:   MongoQueryAtom.h
 * Author: chaozhang
 *
 * Created on April 21, 2014, 9:53 PM
 */

#ifndef MongoQueryAtom_H
#define	MongoQueryAtom_H


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <string>

#include <dlvhex2/PluginInterface.h>
#include <dlvhex2/Error.h>

#include <cstdlib>
#include <iostream>
#include "mongo/client/dbclient.h" // for the driver


namespace dlvhex {
    namespace mongo1 {
        
        class MongoQueryAtom : public PluginAtom
        {
		public:

            MongoQueryAtom(int& i);
	
	virtual
	~MongoQueryAtom();

         
            virtual void
            retrieve(const Query& query, Answer& answer) throw (PluginError);

        };
    }
}



#endif	/* MongoQueryAtom_H */

