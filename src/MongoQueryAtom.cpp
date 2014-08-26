
/**
 * @file MongoQueryAtom.cpp
 * @author Chao ZHANG
 * @date 04/22/2014 21:09:10
 *
 * @brief Defintion of the Mongo Atom.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "MongoQueryAtom.h"
#include <dlvhex2/Registry.h>
#include <dlvhex2/Term.h>

#include "mongo/client/dbclient.h"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace mongo;

namespace dlvhex {
    namespace mongo1 {

        MongoQueryAtom::MongoQueryAtom() : PluginAtom("mongo_query", 1) {

            addInputConstant();
            addInputConstant();
            addInputConstant();
            setOutputArity(10);
        }

        void
        MongoQueryAtom::retrieve(const Query& query, Answer& answer) throw (PluginError) {
            Registry &registry = *getRegistry();
            
            mongo::DBClientConnection c;

            // try the connection of the mongo db at first               
            try {
                
                 c.connect("localhost");
                
                } catch (const mongo::DBException &e) {
                	throw PluginError("MongoDB is not on, or connection breaks!");
            }
                

                const Term& dbc = registry.terms.getByID(query.input[0]);
                const Term& jpattern = registry.terms.getByID(query.input[1]);
                const Term& output = registry.terms.getByID(query.input[2]);


                
                if (!dbc.isQuotedString()) {
                    throw PluginError("Wrong input argument type for input 1, need to be string");
                }


                if (!jpattern.isQuotedString()) {
                    throw PluginError("Wrong input argument type for input 2, need to be string");
                }

                if (!output.isQuotedString()) {
                    throw PluginError("Wrong input argument type for input 3, need to be string");
                }


                std::string s_dbc = dbc.getUnquotedString();
                std::string s_jpattern = jpattern.getUnquotedString();
                std::string s_output = output.getUnquotedString();

             
                    if (s_output == "") {

                        throw PluginError("The output pattern cannot be empty");

                    }
                    
                    // The ouput projection items shuld be seperated by ; 
                    std::vector<string> output_v;
                    boost::split(output_v, s_output, boost::is_any_of(";"));
                    
                    // Start up a cursor to run a tranverse search for the data
                    std::auto_ptr<DBClientCursor> cursor = c.query(s_dbc, fromjson(s_jpattern));

                    if (!cursor.get()) {

                        throw PluginError("query failure");

                    }

                    while (cursor->more()) {

                        Tuple out;
                        
                        BSONObj p = cursor->next();

                        for (size_t i = 0; i < output_v.size(); i++) {
                            
                            std::string in;
                            
                            try {

                                    in = p.getFieldDotted(output_v[i]).String();

                                } catch (DBException &e) {

                            if (p.getFieldDotted(output_v[i]).isNumber()) {
                                
                                ostringstream convert;
                                convert << p.getFieldDotted(output_v[i]).number();
                                in = convert.str();

                            } 
                            else if (p.getFieldDotted(output_v[i]).isBoolean()) {
                                ostringstream convert;

                                if (p.getFieldDotted(output_v[i]).boolean() == true)
                                    in = "true";
                                else
                                    in = "false";

                            }
                            
                            // Check if the element type is Date
                            else if(p.getFieldDotted(output_v[i]).type() == 9){

                                ostringstream convert;
                                
        			convert << p.getFieldDotted(output_v[i]).date();
        			
        		
        			in = convert.str();
        			
        			if(in.size() == 13)
        			 // remove millionsecond to the epoch time stamp
        			 in = in.substr(0,10);
        			

       				}
                            
                            else if(!p.getFieldDotted(output_v[i]).eoo()){

                                
        			in = p.getFieldDotted(output_v[i]).toString(false);
        			
        			in.erase(
                    			remove(in.begin(), in.end(), '\"'),
                   		 	in.end()
                    		        );    
       				
       				}
                            
                            else {
                            
                            	in = "";
                            
                            	}
                            	
                            }
                            
                            //if(!p.hasField(output_v[i]))
                            // {
                            //	cout<< "Some of the document do not have field \"" << output_v[i] << "\"" << endl;
                            //}

                            try {
                                int v;
                                v = boost::lexical_cast<int>(in);
                                Term term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, in);
                                out.push_back(registry.storeTerm(term));

                            } catch (boost::bad_lexical_cast& e) {

                                Term term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, '"' + in + '"');
                                out.push_back(registry.storeTerm(term));
                            }

                        }
                        answer.get().push_back(out);
                    }


            	
        } // end of retrieve function

    } // end of mongo
} // end of dlvhex