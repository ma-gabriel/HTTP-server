
import time
time.sleep(3600)
print("Content-Type: text/html\r\n\r\n")
print("""<!DOCTYPE html>

<html>
<head>
<meta charset="UTF-8">
<title>~/Documents/webserv/Makefile.html</title>
<meta name="Generator" content="Vim/8.2">
<meta name="plugin-version" content="vim8.1_v2">
<meta name="syntax" content="make">
<meta name="settings" content="use_css,pre_wrap,no_foldcolumn,prevent_copy=,use_input_for_pc=fallback">
<meta name="colorscheme" content="none">
<style>
<!--
pre { white-space: pre-wrap; font-family: monospace; color: #ffffff; background-color: #000000; }
body { font-family: monospace; color: #ffffff; background-color: #000000; }
* { font-size: 1em; }
.Identifier { color: #00ffff; font-weight: bold; }
.PreProc { color: #5fd7ff; }
.Statement { color: #ffff00; }
.Comment { color: #00ffff; }
.Constant { color: #ff40ff; }
.Special { color: #ffd7d7; }
-->
</style>
</head>
<body>
<pre id='vimCodeElement'>
<span class="Comment">### config</span>
<span class="Identifier">NAME		</span>= webserv
<span class="Identifier">CXX			</span>= c++
<span class="Identifier">CFLAGS		</span>= -std=c++98 -Wall -Werror -Wextra
<span class="Identifier">DEPFLAGS	</span>= -MMD -MP
<span class="Identifier">AUTHOR		</span>= elleroux
<span class="Identifier">DATE		</span>= 2025/03/31 20:11:19


<span class="Comment">### program files</span>
<span class="Identifier">FILE_EXTENSION	</span>= .cpp
<span class="Identifier">SRCS_PATH		</span>= ./src
<span class="Identifier">INCLUDE_PATH	</span>= ./inc
<span class="Identifier">OBJ_PATH		</span>= ./.obj
<span class="Identifier">SRCS			</span>= <span class="Identifier">$(</span><span class="Statement">wildcard</span><span class="Identifier"> $(SRCS_PATH)/*$(FILE_EXTENSION))</span>
<span class="Identifier">HEADERS			</span>= <span class="Identifier">$(</span><span class="Statement">wildcard</span><span class="Identifier"> $(INCLUDE_PATH)/*.hpp)</span>

<span class="Comment">### objects definition</span>
<span class="Identifier">OBJS </span>= <span class="Identifier">$(</span><span class="Statement">patsubst</span><span class="Identifier"> $(SRCS_PATH)/%$(FILE_EXTENSION),$(OBJ_PATH)/%.o,$(SRCS))</span>
<span class="Identifier">DEPS </span>= <span class="Identifier">$(</span><span class="Statement">patsubst</span><span class="Identifier"> $(SRCS_PATH)/%$(FILE_EXTENSION),$(OBJ_PATH)/%.d,$(SRCS))</span>

<span class="Comment">### Makefile rules</span>
<span class="Identifier">all:</span> <span class="Identifier">$(NAME)</span>

<span class="Identifier">$(NAME):</span> <span class="Identifier">${OBJS}</span>
<span class="Constant">	</span><span class="Identifier">$(CXX)</span><span class="Constant"> </span><span class="Identifier">$(CFLAGS)</span><span class="Constant"> -o </span><span class="Identifier">$@</span><span class="Constant"> </span><span class="Identifier">${OBJS}</span>

<span class="Identifier">$(OBJ_PATH)/%.o:</span> <span class="Identifier">$(SRCS_PATH)</span>/<span class="Identifier">%$(FILE_EXTENSION)</span> <span class="Identifier">$(HEADERS)</span> Makefile
<span class="Constant">	mkdir -p </span><span class="Identifier">$(</span><span class="Statement">dir</span><span class="Identifier"> $@)</span>
<span class="Constant">	</span><span class="Identifier">$(CXX)</span><span class="Constant"> </span><span class="Identifier">$(CFLAGS)</span><span class="Constant"> </span><span class="Identifier">$(DEPFLAGS)</span><span class="Constant"> -c </span><span class="Identifier">$&lt;</span><span class="Constant"> -o </span><span class="Identifier">$@</span><span class="Constant"> -I</span><span class="Identifier">$(INCLUDE_PATH)</span>

<span class="Identifier">clean:</span>
<span class="Special">	@</span><span class="Constant">rm -fr </span><span class="Identifier">$(OBJ_PATH)</span>

<span class="Identifier">fclean:</span> clean
<span class="Special">	@</span><span class="Constant">rm -rf </span><span class="Identifier">$(NAME)</span>

<span class="Identifier">debug:</span> fclean
<span class="Identifier">debug:</span> CFLAGS += -DDEBUG -g3
<span class="Identifier">debug:</span> all

<span class="Identifier">re:</span> fclean
<span class="Special">	@</span><span class="Identifier">$(MAKE)</span><span class="Constant"> --no-print-directory all</span>

<span class="PreProc">-include $(DEPS)</span>
<span class="Statement">.PHONY:</span> all clean fclean re
</pre>
</body>
</html>
""")
