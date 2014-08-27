

/**
 * @file MongoInsertAtom.cpp
 * @author Chao ZHANG
 * @date 04/22/2014 21:09:10
 *
 * @brief Defintion of the Mongo Atom.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "MongoInsertAtom.h"
#include "ConfigFile.h"
#include <dlvhex2/Registry.h>
#include <dlvhex2/Term.h>

#include "mongo/client/dbclient.h"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <ctime>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace mongo;

namespace dlvhex{
    namespace mongo1
    {

        MongoInsertAtom::MongoInsertAtom() : PluginAtom("mongo_insert", 1) {

            addInputTuple();
            setOutputArity(0);
        }

        void
        MongoInsertAtom::retrieve(const Query& query, Answer & answer) throw(PluginError) {
            Registry &registry = *getRegistry();

            mongo::DBClientConnection c;
            ConfigFile cfg("include/config.cfg");
            std::string connection = cfg.getValueOfKey("CONNECTION_STRING");

            // try the connection of the mongo db at first               
            try
            {

                c.connect(connection);

            }

            catch(const mongo::DBException & e) {
                throw PluginError("MongoDB is not on, or connection breaks!");
            }

            int arity = query.input.size();

            if (arity == 1) {

                throw PluginError("Need at least one key and value pair");

            }


            if (arity % 2 == 0) {
                throw PluginError("Key and value are not paired");

            }

            const Term& dbc = registry.terms.getByID(query.input[0]);

            if (!dbc.isQuotedString()) {
                throw PluginError("Wrong input argument type for input 1, need to be string");
            }

            std::string s_dbc = dbc.getUnquotedString();


            BSONObjBuilder b;

            for (int i = 1; i < arity; i = i + 2) {
                ID id1 = query.input[i];
                ID id2 = query.input[i + 1];
                std::stringstream key, value;

                if (id1.isConstantTerm()) {

                    const Term &term = registry.terms.getByID(id1);
                    key << term.getUnquotedString();

                } else {

                    throw PluginError("Wrong input argument type for input: " + i);

                }

                if (id2.isConstantTerm()) {

                    const Term &term = registry.terms.getByID(id2);
                    value << term.getUnquotedString();

                } else if (id2.isIntegerTerm()) {

                    value << id2.address;

                }
                else {

                    throw PluginError("Wrong input argument type for input: " + (i + 1));

                }

                try{

                    try
                    {
                        int v;
                        v = boost::lexical_cast<int>(value.str());
                        b.append(key.str(), v);
                    }

                    catch(boost::bad_lexical_cast & e) {

                        double v;
                        v = boost::lexical_cast<double>(value.str());
                        b.append(key.str(), v);

                    }
                }

                catch(boost::bad_lexical_cast & e) {

                    if (value.str() == "$TIMENOW") {
                    	 //std::time_t t = std::time(NULL); 
                    	 //ostringstream convert;
  			 //convert << t;
  			 //b.append(key.str(),convert.str());
                        b.append(key.str(), jsTime());
                    } else {
                        b.append(key.str(), value.str());
                    }

                }


            } // end of for loop

            BSONObj p = b.obj();

            c.insert(s_dbc, p);

            // check if the insertion is successful     
            std::string e = c.getLastError();
            if (!e.empty()) {
                throw PluginError("insert failed: " + e);
            }

            Tuple out;

            //Term term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, '"' + p.toString() + '"');
            answer.get().push_back(out);


        } // end of retrieve function

    } // end of mongo
} // end of dlvhex
