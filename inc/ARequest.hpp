#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <ostream>

# include <string>
# include <map>

#ifdef DEBUG
#endif

class ARequest
{
public:
// Constructors
	ARequest(void);
	ARequest(const ARequest &from);
// Destructors
	~ARequest(void);
// Overloaded operators
	ARequest &operator=(const ARequest &from);
// Getters
// Setters
// Public member functions
	static void handleRequest(int sock);
	static std::string treatRequest(std::string request);
	virtual void execRequest(void) = 0;



protected:
	std::map<std::string, std::string> _headers;

private:
	struct myRequest
	{
		std::string method;
		std::string target;
		std::string http;
		std::string headers;
		std::string body;
		enum is_working { IS_WORKING = 0, OVER_3 = 1, UNDER_3 = 2,
			NO_HEADERS = 3, NO_RNRN = 4};
		// check is the values placement correspond, not if the values are good
		// checking the goodness is for treatRequest, while the struct is for decomposeRequest
		is_working status;
	};

	myRequest _in;
	static myRequest decomposeRequest(const std::string &request);
};

std::ostream& operator<<(std::ostream& stream, const ARequest& instance);


#endif
