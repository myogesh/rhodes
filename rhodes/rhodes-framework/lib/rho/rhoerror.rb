module Rho
  class RhoError < Exception
    ERR_NONE = 0
    ERR_NETWORK = 1
    ERR_REMOTESERVER = 2
    ERR_RUNTIME = 3
    ERR_UNEXPECTEDSERVERRESPONSE = 4
    ERR_DIFFDOMAINSINSYNCSRC = 5
    ERR_NOSERVERRESPONSE = 6
    ERR_CLIENTISNOTLOGGEDIN = 7
    ERR_CUSTOMSYNCSERVER = 8
    
    attr_reader :code
    
    def initialize(err_code)
        @code = err_code
    end
    
    def message
        ::Rho::RhoError.err_message(code)        
    end
    
    def self.err_message(code)
        if code == ERR_NETWORK
            return "Could not establish network connection."
        elsif code == ERR_REMOTESERVER
            return "Server returned an error."
        elsif code ==  ERR_RUNTIME
            return "Internal error."
        elsif code ==  ERR_UNEXPECTEDSERVERRESPONSE
            return "Unexpected server response."
        elsif code ==  ERR_DIFFDOMAINSINSYNCSRC
            return "All sync sources should be from one domain."
        elsif code ==  ERR_NOSERVERRESPONSE
            return "Could not connect to sync server."
        elsif code ==  ERR_CLIENTISNOTLOGGEDIN
            return "Client is not logged in. No sync will be performed."
        elsif code ==  ERR_CUSTOMSYNCSERVER
            return "Sync server returned application error."
        elsif code ==  ERR_NONE
            return ""
        end
        
        return "Unknown error"
    end
    
  end # RhoError
end # Rho
