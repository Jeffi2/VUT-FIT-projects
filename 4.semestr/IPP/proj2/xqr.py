import argparse
import sys
import os
import xml.dom.minidom as dom


def representsInt(s):
    """Check if s is integer
        Parameters:
            s - string to check
        :return true when is int otherwise false
        """
    try:
        # try to cast
        int(s)
        return True
    except:
        return False


def parseargs():
    """Parse program arguments
        :return dict with parameters
        """
    # argument parse preparation
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input=", type=str, help="Path to input XML file.", dest="input", action="store",
                        default="sys.stdin")
    parser.add_argument("-o", "--output=", type=str, help="Path to output XML file.", dest="output", action="store",
                        default="sys.stdout")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("-q", "--query=", type=str, help="Query for XML filter.", dest="query", action="store")
    group.add_argument("--qf", type=str, help="Path to file with query.", dest="qf", action="store")
    parser.add_argument("-n", help="Not generate a XML head.", action="store_true", dest="n")
    parser.add_argument("-r", "--root=", type=str, help="Name of root element. ", dest="root", action="store")

    # print help
    if len(sys.argv) == 2 and (sys.argv[1] == "-h" or sys.argv[1] == "--help"):
        parser.print_help()
        sys.exit(0)
    elif len(sys.argv) > 1:
        # check if help is not contain in others arguments
        for x in sys.argv[1:]:
            if x == "-h" or x == "--help":
                sys.stderr.write("Bad -h/--help combination!\n")
                sys.exit(1)
    # parse if valid
    try:
        args = parser.parse_args()
    except:
        sys.stderr.write("Bad arguments!\n")
        sys.exit(1)
    paramcounter = 0
    checkedparams = {
    }
    # count valid arguments that was set
    if args.input != "sys.stdin":
        paramcounter += 2
    checkedparams['input'] = args.input
    if args.output != "sys.stdout":
        paramcounter += 2
    checkedparams['output'] = args.output
    if args.query is not None:
        paramcounter += 2
        checkedparams['query'] = args.query
    if args.qf is not None:
        paramcounter += 2
        try:
            # ready query file
            with open(os.path.realpath(args.qf), "r") as file:
                data = file.read()
                file.close()
        except:
            sys.stderr.write("Problem with extract query file!\n")
            sys.exit(80)
        checkedparams['query'] = data
    if args.qf is None and args.query is None:
        sys.stderr.write("No query or query file given!\n")
        exit(80)
    if args.root is None:
        checkedparams['root'] = False
    else:
        paramcounter += 2
        checkedparams['root'] = args.root
    if args.n == False:
        checkedparams['n'] = False
    else:
        paramcounter += 2
        checkedparams['n'] = True

    # some valid argument was repeated
    if len(sys.argv)-1 > paramcounter:
        sys.stderr.write("Some program arguments repeated!\n")
        sys.exit(1)
    return checkedparams


def checkQuery(args):
    """Check if qf or query is valid
            Parameters:
                args - dict with checked program arguments
            :return dict with parsed query
            """
    query = args['query']
    # default values for dict
    queryitems = {
        'select': None,
        'from': None,
        'where': None,
        'wherenot': None,
        'whereop': None,
        'wherecon': None,
        'limit': None
    }

    query = query.lstrip()
    # find SELECT
    index = query.find("SELECT ", 0, 8)
    if index != -1:
        query = query[(index + 7):]
    else:
        sys.stderr.write("BAD QUERY\n")
        sys.exit(80)

    # find FROM
    query = query.lstrip()
    index = query.find("FROM")
    if index != -1:
        queryitems['select'] = query[0:index].strip()
        query = query[(index+4):]
        # empty select
        if len(queryitems['select']) == 0:
            sys.stderr.write("BAD QUERY\n")
            sys.exit(80)
    else:
        sys.stderr.write("BAD QUERY\n")
        sys.exit(80)

    # after from no char
    if len(query) == 0:
        queryitems['from'] = ''

    # space or attribute after from
    if query.isspace():
        # only space
        queryitems['from'] = ''
    else:
        # attribute
        query = query.lstrip()
        if (query.find("WHERE") == -1) and (query.find("LIMIT ") == -1):
            queryitems['from'] = query.strip()
        else:
            # parse WHERE
            index = query.find("WHERE")
            if index != -1:
                # WHERE found
                queryitems['from'] = query[0:index].strip()
                query = query[(index+5):].lstrip()
                # find (NOT )*
                notcount = 0
                notindex = query.find("NOT ", 0, 4)
                if notindex != -1:
                    while notindex != -1:
                        notcount += 1
                        query = query[4:].lstrip()
                        notindex = query.find("NOT ", 0, 4)
                # NOT NOT == true
                if notcount % 2 == 0:
                    queryitems['wherenot'] = False
                else:
                    queryitems['wherenot'] = True
                # find contains | < | > | =
                con = True
                delimiterIndex = query.find("CONTAINS")
                if delimiterIndex == -1:
                    con = False
                    delimiterIndex = query.find("<")
                    if delimiterIndex == -1:
                        delimiterIndex = query.find(">")
                        if delimiterIndex == -1:
                            delimiterIndex = query.find("=")
                            if delimiterIndex == -1:
                                sys.stderr.write("BAD QUERY\n")
                                sys.exit(80)
                if delimiterIndex == 0:
                    sys.stderr.write("BAD QUERY\n")
                    sys.exit(80)
                # where element
                queryitems['where'] = query[0:delimiterIndex].strip()
                if con == True:
                    query = query[delimiterIndex+8:].lstrip()
                    queryitems['whereop'] = "CONTAINS"
                else:
                    queryitems['whereop'] = query[delimiterIndex]
                    query = query[delimiterIndex+1:].lstrip()
                # is limit set or not?
                limitIndex = query.find("LIMIT ")
                if limitIndex == -1:
                    stringOrNumber = query.strip()
                else:
                    stringOrNumber = query[0:limitIndex].strip()
                    query = query[limitIndex:].lstrip()
                if len(stringOrNumber) == 0:
                    sys.stderr.write("BAD QUERY\n")
                    sys.exit(80)
                # check if where parameter is valid
                if (stringOrNumber[0] == "\"" and stringOrNumber[len(stringOrNumber)-1] == "\"") or \
                        (stringOrNumber[0] == "\'" and stringOrNumber[len(stringOrNumber) - 1] == "\'"):
                    queryitems['wherecon'] = stringOrNumber
                elif representsInt(stringOrNumber):
                    if con == True:
                        sys.stderr.write("BAD QUERY\n")
                        sys.exit(80)
                    queryitems['wherecon'] = stringOrNumber
                else:
                    sys.stderr.write("BAD QUERY\n")
                    sys.exit(80)
            # limit set without where
            index = query.find("LIMIT ")
            if index != -1:
                # limit found
                if queryitems['from'] == None:
                    queryitems['from'] = query[0:index].strip()
                query = query[(index + 6):].strip()
                if representsInt(query):
                    queryitems['limit'] = query
                    if int(query) < 0:
                        sys.stderr.write("BAD QUERY\n")
                        sys.exit(80)
                else:
                    sys.stderr.write("BAD QUERY\n")
                    sys.exit(80)
    return queryitems


def getInputXML(args):
    """Load and parse XML file
            Parameters:
                 - args - dict with checked program arguments
            :return xml minidom structure
            """
    # stdin
    if args['input'] == "sys.stdin":
        xmlString = ""
        # read all file
        for line in sys.stdin:
            xmlString += line
        try:
            # parse xml
            xmldata = dom.parseString(xmlString)
        except:
            sys.stderr.write("Problem with XML stdin read!\n")
            sys.exit(4)
    else:
        try:
            # check if file exists
            with open(os.path.realpath(args['input']), "r") as file:
                file.close()
        except:
            sys.stderr.write("Problem with open XML file!\n")
            sys.exit(2)
        try:
            # parse xml data
            xmldata = dom.parse(os.path.realpath(args['input']))
        except:
            sys.stderr.write("Problem with parse XML file!\n")
            sys.exit(4)
    return xmldata


def checkElements(query):
    """Check if element / attribute or element.attribute in FROM and WHERE
        Parameters:
             - query - dict with parsed query
        :return updated query
        """
    query['fromEl'] = None
    query['fromAt'] = None
    # find . in string
    dotIndex = query['from'].find(".")
    # from only element
    if dotIndex == -1:
        query['fromEl'] = query['from']
    elif dotIndex == len(query['from'])-1:
        # only element. set
        sys.stderr.write("BAD QUERY\n")
        sys.exit(80)
    elif dotIndex == 0:
        # only .attribute
        query['fromAt'] = query['from'][1:]
    else:
        # element.attribute
        query['fromEl'] = query['from'][0:dotIndex]
        query['fromAt'] = query['from'][dotIndex + 1:]
    query['whereEl'] = None
    query['whereAt'] = None
    # check if where have element / attribute or both
    if query['where'] is not None:
        dotIndex = query['where'].find(".")
        if dotIndex == -1:
            # element
            query['whereEl'] = query['where']
        elif dotIndex == len(query['where'])-1:
            # element.
            sys.stderr.write("BAD QUERY\n")
            sys.exit(80)
        elif dotIndex == 0:
            # .attribute
            query['whereAt'] = query['where'][1:]
        else:
            # element.attribute
            query['whereEl'] = query['where'][0:dotIndex]
            query['whereAt'] = query['where'][dotIndex + 1:]
    # delete old values
    del query['where']
    del query['from']
    return query


def checkNot(query):
    """If odd count of "NOT" then negate relation operators
        Parameters:
            - query - dict with parsed query
        :return updated query
        """
    # if odd count of NOT then negate operator
    if query['wherenot'] == False:
        return query
    else:
        if query['whereop'] == "=":
            query['whereop'] = "!"
        elif query['whereop'] == "<":
            query['whereop'] = '>'
        elif query['whereop'] == ">":
            query['whereop'] = "<"
        elif query['whereop'] == "CONTAINS":
            query['whereop'] = "NCONTAINS"
        return query


def testCondition(query, element):
    """Testing if element or attribute value and condition are ok
        Parameters:
            - query - dict with parsed query
            - element - text value of element or attribute
        :return true when condition ok otherwise false
        """
    # test if elements or attribute value and condition in where are ok
    if query['whereop'] == "CONTAINS" or query['whereop'] == "NCONTAINS":
        if isinstance(element, str):
            # remove " " / ' '
            if query['wherecon'][0] == "\"" or query['wherecon'][0] == "\'":
                query['wherecon'] = query['wherecon'][1:len(query['wherecon'])-1]

            # find substring in string
            findIndex = element.find(query['wherecon'])
            if query['whereop'] == "CONTAINS" and findIndex != -1:
                return True
            if query['whereop'] == "NCONTAINS" and findIndex == -1:
                return True
            return False
        else:
            return False
    else:
        if query['wherecon'][0] != "\"" and query['wherecon'][0] != "\'":
            # number in query
            queryvalue = int(query['wherecon'])
            try:
                elementvalue = float(element)
            except:
                # bad type of element/attribute value
                return False
        else:
            # compare strings
            queryvalue = query['wherecon'][1:len(query['wherecon'])-1]
            elementvalue = element
        # compare values (strings or numbers)
        if query['whereop'] == "<":
            return elementvalue < queryvalue
        if query['whereop'] == ">":
            return elementvalue > queryvalue
        if query['whereop'] == "=":
            return elementvalue == queryvalue
        if query['whereop'] == "!":
            return elementvalue != queryvalue


def processWhere(query, data):
    """Filter data elements according to condition and where element/attribute
        Parameters:
            - query - dict with parsed query
            - data - elements under FROM
        :return list with filtered elements
        """
    final = []
    query = checkNot(query)
    # where element
    if query['whereEl'] is not None and query['whereAt'] is None:
        # only element in query
        for e in data:
            # check if element value and condition are ok
            if e.tagName == query['whereEl'] and testCondition(query, e.firstChild.nodeValue) == True:
                final.append(e)
            # for every sub-element check if value and condition are ok
            for i in e.getElementsByTagName(query['whereEl']):
                for j in i.childNodes:
                    # element does not has text value, has sub-elements
                    if j.__class__.__name__ == "Element":
                        sys.stderr.write("BAD XML, element contains subelement insted of text or float.\n")
                        sys.exit(4)
                # check condition
                if testCondition(query, i.firstChild.nodeValue) == True:
                    final.append(e)
    # only .attribute
    elif query['whereEl'] is None and query['whereAt'] is not None:
        for e in data:
            # check if element has attribute that i want
            if e.hasAttribute(query['whereAt']) and testCondition(query, e.getAttribute(query['whereAt'])) == True:
                final.append(e)
            # check all sub-elements
            for i in e.childNodes:
                if i.__class__.__name__ == "Element":
                    # find atrribute and check value
                    if i.hasAttribute(query['whereAt']) and testCondition(query, i.getAttribute(query['whereAt'])) == True:
                        final.append(e)
    # element.attribute
    else:
        for e in data:
            # check "from" element
            if e.tagName == query['whereEl']:
                # have attribute that i want?
                if e.hasAttribute(query['whereAt']) and testCondition(query, e.getAttribute(query['whereAt'])) == True:
                    final.append(e)
            # check sub-elements
            for i in e.getElementsByTagName(query['whereEl']):
                if i.hasAttribute(query['whereAt']) and testCondition(query, i.getAttribute(query['whereAt'])) == True:
                    final.append(e)
    return final


def invokeRecursion(all, top):
    """Invoke recursive search of all document elements
        Parameters:
            - all - dict for result
            - top - root element of xml document
        :return list with all elements
        """
    # find all elements
    recursionSeach(top, all)
    return all


def recursionSeach(top, all):
    """Recursive search in all child nodes.
        Parameters:
            - all - dict for result
            - top - root element of xml document
        :return list with all elements
        """
    # find all sub-element children
    for node in top.childNodes:
        # if text continue
        if node.__class__.__name__ != "Element":
            continue
        else:
            # add element
            all.append(node)
        # check all sub-elements
        for sub in node.childNodes:
            if sub.__class__.__name__ == "Element":
                # add element
                temp = sub
                all.append(temp)
                # recursive for sub-element
                recursionSeach(temp, all)


def filterXML(filtered, query, data):
    """Filter whole XML
        Parameters:
            - query - dict with parsed query
            - data - xml file
            - filtered - list for result
        :return list with elements that should be in output file
        """
    rootSel = False
    # find root
    root = None
    if query['fromEl'] == '':
        return filtered
    elif query['fromEl'] == "ROOT":
        # because xml read ok then it have root so no need to try
        if data.documentElement.tagName == query['select']:
            filtered = [data.documentElement]
            rootSel = True
        root = [data.documentElement]
    elif query['fromEl'] is not None and query['fromAt'] is None:
        # try to find root elem
        try:
            root = [data.getElementsByTagName(query['fromEl'])[0]]
        except:
            # root not found
            root = None
            return filtered
    elif query['fromEl'] is None and query['fromAt'] is not None:
        # try to find attribute
        try:
            top = data
            all = []
            # all elements in xml
            all = invokeRecursion(all, top)

            for e in all:
                if e.hasAttribute(query['fromAt']):
                    root = [e]
                    break
        except:
            # root not found
            root = None
            return filtered
    else:
        # find element and atribute
        try:
            elements = data.getElementsByTagName(query['fromEl'])
            for e in elements:
                if e.hasAttribute(query['fromAt']):
                    root = [e]
                    break
        except:
            # root not found
            root = None
            return filtered
    # root not found hot fix
    if root is None:
        return filtered

    if rootSel == False:
        for e in root:
            # select all from root that match by the name
            filtered.extend(e.getElementsByTagName(query['select']))
    if not filtered:
        return filtered
    # where in query
    if query['whereop'] is not None:
        filtered = processWhere(query, filtered)
    # limit in query
    if query['limit'] is not None:
        if int(query['limit']) < len(filtered):
            return filtered[:int(query['limit'])]
    return filtered


# control whole program flow
if __name__ == '__main__':
    # parse arguments
    args = parseargs()
    # parse and validate query
    parsedQuery = checkQuery(args)
    # load xml data
    xmlData = getInputXML(args)
    # check FROM and WHERE values
    parsedQuery = checkElements(parsedQuery)
    # prepare output file
    if args['output'] == "sys.stdout":
        output = sys.stdout
    else:
        try:
            output = open(os.path.realpath(args['output']), "w")
        except:
            sys.stderr.write("Can not create output file!\n")
            sys.exit(3)
    # not generate xml head
    if args['n'] == False:
        output.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
    # generate root element
    if args['root'] != False:
        output.write("<" + args['root'] + ">\n")
    # filter elements
    filteredXML = []
    filteredXML = filterXML(filteredXML, parsedQuery, xmlData)
    # write valid elements to file
    for e in filteredXML:
        output.write(e.toxml())
    # generate /root element
    if args['root'] != False:
        output.write("</" + args['root'] + ">")
    # endline of file
    output.write("\n")
    output.close()
    sys.exit(0)
