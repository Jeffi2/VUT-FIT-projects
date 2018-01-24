<?php
/**
 * Created by PhpStorm.
 * User: filip
 * Date: 5.11.17
 * Time: 11:00
 */
namespace App\Model;
use Nette\Neon\Exception;
use Nette\Object;
use Nette\Utils\Strings;
use Nette\Security\Passwords;

class loginManager extends \Nette\Object implements \Nette\Security\IAuthenticator{

    private $database;
    private $result;

    public function __construct(\Nette\Database\Context $database){

        $this->database = $database;
    }

    public function login($username,$password)
    {
        $result = $this->database->query('SELECT login,type, can_log,heslo FROM users WHERE login=? ', $username);
        if ($result->getRowCount() === 0) {
            return null;
        } else {
            $result = $result->fetchAll();
            if (Passwords::verify($password,$result[0]["heslo"])){
                return $result;
            }
            else {
                return null;
            }
        }
    }
    public function authenticate(array $credentials){

       $username=$credentials[0];
       $password=$credentials[1];
       $result=self::login($username,$password);
       if ($result == null)
           throw new \Nette\Security\AuthenticationException("Špatně zadané přihlašovací jméno nebo heslo!", 1);
       $permission="";
       switch($result[0]["type"]){
           case 1: $permission="admin";
                    break;
           case 2: $permission="staff";
                    break;
           case 3: $permission="supplier";
                    break;
           default:
               throw new Exception("unknown permission ",1);
       }
       if ($result[0]["can_log"] == 0) {
           throw new \Nette\Security\AuthenticationException("Login byl zablokován!", 1);
       }
        return new \Nette\Security\Identity($result,$permission);
    }
}