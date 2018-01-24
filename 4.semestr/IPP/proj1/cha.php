<?php
/**
 * CHA - xjudap00
 * @file cha.php
 * @autor Petr Juda
 */
//Global variables for every function
$OK = 0;
$ERR_ARG = 1;
$ERR_OPEN = 2;
$ERR_WRITE = 3;
$ERR_UNSPEC = 100;
$space_count = 0;
$function_name = array();

/**
 * print_help
 *
 * Prints --help to user.
 *
 */
function print_help () {
    echo "Program generuje XML strukturu hlavičkových souborů .h psaných ve standardu C99. Program se ovládá pomocí přepínačů: \n
    --input=fileordir \t Zadejte cestu k vstupnímu souboru nebo složce. Bez parametru se analyzují soubory v ./\n
    --output=filename \t Zadajte cestu k výstupnímu souboru. Bez parametru se vypisuje na STDOUT.\n
    --pretty-xml=k \t Určuje odsazení XML dokumentu. Bez zadání =k se použije výchozí hodnota 4.\n
    --no-inline \t Program bude ignorovat funkce definované jako inline.\n
    --max-par=n \t Program bude zpracovávat pouze funkce s maximálně n parametry.\n
    --no-duplicates \t Program bude ignorovat funkce se stejným jménem. Vypíše pouze první nalezenou.\n
    --remove-whitespace \t Odstranění přebytečných bílých znaků z návratového typu funkce a parametrů.\n
    ";
    print "\n";
}

/**
 * Function check_args
 *
 * Control and validate program arguments after getopt.
 *
 * @param $args array arguments from getopt
 * @return array with checked_args
 *
 */
function check_args ($args) {
    //ERR codes
    global $ERR_ARG;
    global $ERR_OPEN;
    global $ERR_WRITE;
    //Help param is already checked in run()
    //Array for validated args
    $checked_args = array();
    //is input set?
    if (isset($args["input"])) {
        //check if input is file / dir can be open
        if (realpath($args["input"]) == true) {
            $checked_args["input"] = realpath($args["input"]);
            $checked_args["input-original"] = $args["input"];
        }
        else {
            fwrite(STDERR, "Input dir or file unreachable\n");
            exit($ERR_OPEN);
        }
    }
    else {
        //check if input dir can be open -> this should work every time
        if (realpath("./") == true) {
            $checked_args["input"] = realpath("./");
            $checked_args["input-original"] = "./";
        }
        else {
            fwrite(STDERR, "Home dir unreachable\n");
            exit($ERR_OPEN);
        }
    }
    //is output set?
    if (isset($args["output"])) {
        //Check if output is dir
        if (is_dir(realpath($args["output"])) === true) {
            fwrite(STDERR, "Output if directory not a file!\n");
            exit($ERR_WRITE);
        }
        else {
            //should be file
            //is the last char in path / ? -> so you try to joke me, exit
            $test_string_path = $args["output"];
            if ($test_string_path[strlen($test_string_path)-1] == "/") {
                fwrite(STDERR, "Output path is to directory.\n");
                exit($ERR_WRITE);
            }
            //check if parent dir of file exists
            $output_path = dirname($args["output"]);
            if (realpath($output_path) == false) {
                fwrite(STDERR, "Output directory does not exists!\n");
                exit($ERR_WRITE);
            }
            if (is_writable($output_path) == false) {
                fwrite(STDERR, "Output directory is not writable!\n");
                exit($ERR_WRITE);
            }
            $output_path = realpath($output_path);
            $output_path .= "/";
            $output_path .= basename($args["output"]);
            $checked_args["output"] = $output_path;
        }
    }
    else {
        //output is not set then write to STDOUT
        $checked_args["output"] = "STDOUT";
    }
    //is pretty-xml set?
    if (isset($args["pretty-xml"])) {
        // =k not set, use default 4
        if ($args["pretty-xml"] === false) {
            $checked_args["pretty-xml"] = 4;
        }
        else {
            // = k is set, check if is it number
            if (is_numeric($args["pretty-xml"])) {
                $checked_args["pretty-xml"] = $args["pretty-xml"];
            }
            // = k is not a number -> exit
            else {
                fwrite(STDERR, "Invalid pretty-xml arg!\n");
                exit($ERR_ARG);
            }
        }
    }
    //pretty not set -> no layout
    else {
        $checked_args["pretty-xml"] = false;
    }
    //is no-inline set?
    if (isset($args["no-inline"])) {
        $checked_args["no-inline"] = true;
    }
    else {
        $checked_args["no-inline"] = false;
    }
    //is max-par=n set?
    if (isset($args["max-par"])) {
        //n is set, check if it is a number
        if (is_numeric($args["max-par"])) {
            $checked_args["max-par"] = $args["max-par"];
        }
        //n is not a number -> exit
        else {
            fwrite(STDERR, "Invalid max-par arg!\n");
            exit($ERR_ARG);
        }
    }
    else {
        //not set
        $checked_args["max-par"] = false;
    }
    //is no-duplicates set?
    if (isset($args["no-duplicates"])) {
        $checked_args["no-duplicates"] = true;
    }
    else {
        $checked_args["no-duplicates"] = false;
    }
    //is remove-whitespace set?
    if (isset($args["remove-whitespace"])) {
        $checked_args["remove-whitespace"] = true;
    }
    else {
        $checked_args["remove-whitespace"] = false;
    }
    return $checked_args;
}

/**
 * Function print_xml
 *
 * Prints xml to file and checking --pretty-xml
 *
 * @param resource $file file descriptor
 * @param $checked_args array validated arguments
 * @param $string string String that should be written to xml file
 * @param $format boolean Set if should print spaces
 *
 */
function print_xml ($file, $checked_args, $string, $format) {
    global $space_count;
    //is pretty-xml set?
    if ($checked_args["pretty-xml"] === false) {
        //no layout
        fwrite($file, $string);
        fflush($file);
    }
    elseif ($format == true) {
        //print with spaces
        for ($i = 0; $i < $space_count; $i++) {
            fwrite($file, " ");
            fflush($file);
        }
        fwrite($file, $string."\n");
        fflush($file);
    }
    else {
        //print without spaces
        fwrite($file, $string."\n");
        fflush($file);
    }
}

/**
 * parse_function
 *
 * Parsing function type, name and arguments
 *
 * @param $file_write resource file descripto
 * @param $checked_args array validated arguments
 * @param $function string one extracted function from a file
 * @param $file_path string path to file that contains this function
 *
 */
function parse_function($file_write, $checked_args, $function, $file_path) {
    global $function_name;
    global $can_write ;
    $can_write = true;
    //parse name and type
    preg_match_all("/\s*[\s\w\*\(\)]+\w+\s*\(/", $function, $rettyp_name_array);
    //should be only one in array, so can do it
    $parse_rettyp_name = $rettyp_name_array[0][0];
    $parse_rettyp_name = trim($parse_rettyp_name);

    //parse_rettyp_name is rettype with name atd the end -> parse name
    $parse_rettyp_name = rtrim($parse_rettyp_name, "(");
    $parse_rettyp_name = trim($parse_rettyp_name);

    //parse name from string
    preg_match("/\w*$/", $parse_rettyp_name, $last_word);
    //there is name of function
    $name = $last_word[0];
    //cut name from return type
    $rettype = rtrim($parse_rettyp_name, $name);
    $rettype = trim($rettype);
    //replace \n and other with " "
    $chars_for_replace = array("\r", "\n", "\t", "\f", "\v");
    $rettype = str_replace($chars_for_replace, " ", $rettype);
    //if --remove-whitespace set then remove multiple whitespace
    if ($checked_args["remove-whitespace"] == true) {
        //remove multiple spaces
        $rettype = preg_replace("/\s+/", " ", $rettype);
        //fix for remove space before pointers
        $rettype = preg_replace("/ \*/", "*", $rettype);
        $rettype = preg_replace("/\* /", "*", $rettype);
    }
    //check for --no-inline param
    if ($checked_args["no-inline"] == true) {
        //is function inline?
        preg_match("/.*inline.*/", $rettype, $is_inline);
        if (count($is_inline) > 0) {
            $can_write = false;
        }
    }
    //array with functions names
    if (count($function_name) == 0) {
        array_push($function_name, $name);
    }
    else {
        $is_in_array = false;
        for ($i = 0; $i < count($function_name); $i++) {
            if ($function_name[$i] == $name) {
                $is_in_array = true;
            }
        }
        if ($checked_args["no-duplicates"] == false) {
            array_push($function_name, $name);
        }
        else {
            if ($is_in_array == false) {
                array_push($function_name, $name);
            }
            else {
                $can_write = false;
            }
        }
    }
    //PARSE PARAMETERS
    preg_match_all("/\([\s\w\,\.\-\>\*\[\]]*\)\s*\;/", $function, $params_array);
    $parse_params = $params_array[0][0];
    //remove whitespace
    $parse_params = rtrim($parse_params, ";");
    $parse_params = ltrim($parse_params, "(");
    $parse_params = trim($parse_params);
    $parse_params = rtrim($parse_params, ")");
    $parse_params = trim($parse_params);
    //array for parameters
    $params = array();
    global $varargs;
    $varargs = "no";
    global $cleared_types;
    //clear global variable
    $fix_count = count($cleared_types);
    for ($j = 0; $j < $fix_count; ++$j) {
        unset($cleared_types[$j]);
    }
    if (strlen(trim($parse_params)) > 0) {
        //has function some param?
        if (strpos($parse_params, ",") === false) {
            //only 1 param
            $only_one_param = trim($parse_params);
            if ($only_one_param == '...') {
                //is it vararg
                $varargs = "yes";
            }
            else {
                //not vararg -> push to array
                array_push($params, $only_one_param);
            }
        }
        else {
            //have move params -> explode
            $params = explode(",", $parse_params);
        }
        for ($i = 0; $i < count($params); $i++) {
            //remove whitespace
            $params[$i] = trim($params[$i]);
        }
        $cleared_types = array();
        $cleared_names = array();
        for ($i = 0; $i < count($params); $i++) {
            if ($params[$i] == '...') {
                //is it vararg
                $varargs = "yes";
                break;
            }
            preg_match("/\w*$/", $params[$i], $last);
            //THERE IS NAME OF PARAMETER
            $param_name = $last[0];
            $type = rtrim($params[$i], $param_name);
            $type = trim($type);
            //replace \n and other with " "
            $chars_for_replace = array("\r", "\n", "\t", "\f", "\v");
            $type = str_replace($chars_for_replace, " ", $type);
            //if --remove-whitespace set then remove multiple whitespace
            if ($checked_args["remove-whitespace"] == true) {
                $type = preg_replace("/\s+/", " ", $type);
                $type = preg_replace("/ \*/", "*", $type);
                $type = preg_replace("/\* /", "*", $type);
            }
            //fix void function
            if ($param_name == "void") {
                $tmp = $type;
                $type = $param_name;
                $param_name = $tmp;
            }
            else {
                array_push($cleared_names, $param_name);
                array_push($cleared_types, $type);
            }
        }
        if ($checked_args["max-par"] !== false) {
            //control number of function parameters
            if (count($cleared_types) > $checked_args["max-par"]) {
                $can_write = false;
            }
        }
    }
    //spaces between XML elements
    global $space_count;
    //XML write <function>
    if ($can_write == true) {
        print_xml($file_write, $checked_args, "<function file=\"$file_path\" name=\"$name\" varargs=\"$varargs\" rettype=\"$rettype\">", true);
        for ($i = 0; $i < count($cleared_types); $i++) {
            if ($i == 0) {
                $space_count *= 2;
            }
            $good_counter = $i;
            $good_counter++;
            //XML write <param>
            print_xml($file_write, $checked_args, "<param number=\"$good_counter\" type=\"$cleared_types[$i]\" />", true);
            if ($i == (count($cleared_types) - 1)) {
                $space_count /= 2;
                print_xml($file_write, $checked_args, "</function>", true);
            }
        }
        if (count($cleared_types) == 0) {
            print_xml($file_write, $checked_args, "</function>", true);
        }
    }
}

/**
 * file_extract
 *
 * load data from file and parsing functions from it
 *
 * @param $file_write resource file for output
 * @param $checked_args array validated arguments
 * @param $file resource file with functions
 * @param $dir_bool boolean file or directory set in params
 *
 */
function file_extract($file_write, $checked_args, $file, $dir_bool) {
    global $ERR_OPEN;
    //open a header file
    $opened_file = file_get_contents($file);
    if ($opened_file === false) {
        fwrite(STDERR, "Can not open a file!\n");
        exit($ERR_OPEN);
    }
    if ($dir_bool == true) {
        $file_path = $file->getPathname();
        //edited original path will be used for cut of front test_char
        $edited_original_path = $checked_args["input"];
        $test_char = $checked_args["input"][strlen($checked_args["input"])-1];
        if ($test_char != "/") {
            $edited_original_path .= "/";
        }
        //relative path for print
        $file_path = substr($file_path, strlen($edited_original_path));
    }
    else {
        $file_path = $checked_args["input-original"];
    }
    //clear the file
    $filtered_file = clear_rubbish($opened_file);
    //looking for function construction
    preg_match_all("/\s*[\s\w\*]+\w+\s*\([\s\w\,\.\-\>\*\[\]]*\)\s*\;/si", $filtered_file, $multi_array);

    //extracted functions from file
    if (count($multi_array[0]) > 0) {
        for ($i = 0; $i < count($multi_array[0]) ; $i++) {
            $parsed_header_array[0][$i] = trim($multi_array[0][$i]);
            //extract type, name and params from function
            parse_function($file_write, $checked_args, $parsed_header_array[0][$i], $file_path);
        }
    }
}

/**
 * clear_rubbish
 *
 * FSM for remove comments, macros and strings
 *
 * @param $file string opened file
 * @return string cleared input data
 *
 */
function clear_rubbish($file) {
    //for clean i can use char by char search even that it does not support multibyte chars because i am looking for standard chars
    $fsm_state = 0;
    $cur_counter = 0;
    $filtered_file = "";
    for ($i = 0; $i < strlen($file); $i++ ) {
        $char = $file[$i];
        switch ($fsm_state) {
            case 0:
                //delete functions body
                if ($char == "{") {
                    $cur_counter++;
                    $fsm_state = 1;
                }
                //delete comments
                elseif ($char == "/") {
                    $fsm_state = 2;
                }
                //delete macros
                elseif ($char == "#") {
                    $fsm_state = 6;
                }
                //delete strings
                elseif ($char == "\"" || $char == '\'') {
                    $fsm_state = 9;
                }
                else {
                    $filtered_file .= $char;
                }
                break;
            case 1:
                if ($char == "}") {
                    $cur_counter--;
                    if ($cur_counter == 0) {
                        $fsm_state = 0;
                        //from definition create declaration
                        $filtered_file .= ';';
                    }
                }
                else {
                    if ($char == "{") {
                        $cur_counter++;
                    }
                }
                break;
            case 2:
                if ($char == "*") {
                    $fsm_state = 4;
                }
                elseif ($char == "/") {
                    $fsm_state = 3;
                }
                else {
                    $filtered_file .= "/";
                    $filtered_file .= $char;
                    $fsm_state = 0;
                }
                break;
            case 3:
                if ($char == "\n") {
                    $fsm_state = 0;
                }
                elseif ($char == "\\") {
                    $fsm_state = 8;
                }
                break;
            case 4:
                if ($char == "*") {
                    $fsm_state = 5;
                }
                break;
            case 5:
                if ($char == "/") {
                    $fsm_state = 0;
                }
                break;
            case 6:
                if ($char == "\\") {
                    $fsm_state = 7;
                }
                elseif ($char == "\n") {
                    $fsm_state = 0;
                }
                break;
            case 7:
                if (ctype_space($char) == true) {
                    $fsm_state = 7;
                }
                else {
                    $fsm_state = 6;
                }
                break;
            case 8:
                if (ctype_space($char) == true) {
                    $fsm_state = 8;
                }
                else {
                    $fsm_state = 3;
                }
                break;
            case 9:
                if ($char == "\\") {
                    $fsm_state = 10;
                }
                elseif ($char == "\"" || $char == '\'') {
                    $fsm_state = 0;
                }
                break;
            case 10:
                $fsm_state = 9;
                break;
        }
    }
    return $filtered_file;
}

/**
 * run
 *
 * main function that control program flow
 *
 * @return int exit code
 */
function run () {
    //include global error codes
    global $argc;
    global $OK;
    global $ERR_ARG;
    global $ERR_UNSPEC;
    global $ERR_WRITE;
    //set utf-8
    if (mb_internal_encoding("UTF-8") != TRUE) {
        fwrite(STDERR, "Set UTF-8 failed!\n");
        exit($ERR_UNSPEC);
    }
    //array for getopt
    $allowed_args = array (
        "help",
        "input:",
        "output:",
        "pretty-xml::",
        "no-inline",
        "max-par:",
        "no-duplicates",
        "remove-whitespace",
    );
    $args = getopt(NULL, $allowed_args);
    //getopt success check
    if ($args === false) {
        fwrite(STDERR, "Getopt parse args failed!\n");
        exit($ERR_ARG);
    }
    //same args repeted or some unsupported
    if (sizeof($args) != $argc - 1) {
        fwrite(STDERR, "Arg repeted! or unsupported!\n");
        exit($ERR_ARG);
    }
    //help set
    if (isset($args["help"])) {
        if ($argc === 2) {
            print_help();
            return $OK;
        }
        else {
            fwrite(STDERR, "HELP arg is not only!\n");
            exit($ERR_ARG);
        }
    }
    //validate arguments
    $checked_args = check_args($args);
    //Open file for building a XML document
    if ($checked_args["output"] == "STDOUT") {
        //write to STDOUT
        //i can do i because program is not reading from stdout any time
        $file_write = fopen('php://stdout', 'w');
        if ($file_write == false) {
            fwrite(STDERR, "Can not open output!\n");
            exit($ERR_WRITE);
        }
    }
    else {
        //write to file
        $file_write = fopen($checked_args["output"], 'w');
        if ($file_write == false) {
            fwrite(STDERR, "Can not open output!\n");
            exit($ERR_WRITE);
        }
    }
    //set global spaces before elements
    global $space_count;
    if (is_numeric($checked_args["pretty-xml"])) {
        $space_count = $checked_args["pretty-xml"];
    }
    //print head of XML file
    print_xml($file_write, $checked_args, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>", false);
    //Find all headers if directory is set or parse only one file
    if (is_file($checked_args["input"]) == true) {
        //only one file set
        print_xml($file_write, $checked_args, "<functions dir=\"\">", false);
        file_extract($file_write, $checked_args, $checked_args["input"], false);
    }
    else {
        //directory set
        //check if path to file ends with /
        $test_char = $checked_args["input-original"][strlen($checked_args["input-original"])-1];
        if ($test_char != "/") {
            //add /
            $checked_args["input-original"] .= "/";
        }
        print_xml($file_write, $checked_args, "<functions dir=\"".$checked_args["input-original"]."\">", false);
        $find_headers = new RecursiveDirectoryIterator($checked_args["input"]);
        //search all
        foreach (new RecursiveIteratorIterator($find_headers) as $file) {
            if (preg_match("/.*\.h$/i", $file)) {
                //call to process file
                // echo "Name: $file\n";
                file_extract($file_write, $checked_args, $file, true);
            }
        }
    }
    //write end of XML
    fwrite($file_write, "</functions>\n");
    fflush($file_write);
    fclose($file_write);
    return 0;
}

/**
 * DEBUG_array_print
 *
 * debug function that print key -> value from array
 *
 * @param $print_array array to print
 *
 */
function DEBUG_array_print ($print_array) {
    //DEBUG ARRAY PRINT
    print ("\n");
    print ("KEY\t\t\t\t\tVALUE\n");
    print ("------------------->\n");
    foreach ($print_array as $key => $value) {
        print ($key ."\t\t\t\t". $value . "\n");
    }
    print ("------------------->\n");
}

run();

?>