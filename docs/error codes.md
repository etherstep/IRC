### Handshake & Welcome Sequence

| Value | Macro | Explanation |
| :--- | :--- | :--- |
| **1** | `RPL_WELCOME` | Sent after successful `NICK`/`USER` registration. Contains the full `nick!user@host` identifier. |
| **2** | `RPL_YOURHOST` | Informs the client of your server's name and the software version it is running. |
| **3** | `RPL_CREATED` | Tells the client the date/time your server software was compiled or started. |
| **4** | `RPL_MYINFO` | Lists the server name, version, and the specific user and channel modes your server supports. |

### Error Codes

| Value   | Macro                  | Explanation                                                                                                        |
| :------ | :--------------------- | :----------------------------------------------------------------------------------------------------------------- |
| **401** | `ERR_NOSUCHNICK`       | The target nickname for a command (like `PRIVMSG`) does not exist or is offline.                                   |
| **403** | `ERR_NOSUCHCHANNEL`    | The target channel for a command (like `JOIN` or `PRIVMSG`) does not exist.                                        |
| **404** | `ERR_CANNOTSENDTOCHAN` | The user lacks permissions to send a message to a channel (e.g., they aren't joined, or the channel is moderated). |
| **421** | `ERR_UNKNOWNCOMMAND`   | The server parsed a valid command format, but does not recognize the command word itself.                          |
| **431** | `ERR_NONICKNAMEGIVEN`  | A command (like `NICK`) was sent without the required nickname parameter attached.                                 |
| **432** | `ERR_ERRONEUSNICKNAME` | The requested nickname contains invalid characters (like spaces or special symbols).                               |
| **433** | `ERR_NICKNAMEINUSE`    | The requested nickname is already claimed by another connected client.                                             |
| **451** | `ERR_NOTREGISTERED`    | The client tried to use a command (like `JOIN` or `PRIVMSG`) before completing the `NICK`/`USER` handshake.        |
| **461** | `ERR_NEEDMOREPARAMS`   | The command was recognized, but the client didn't send enough arguments to execute it.                             |
| **462** | `ERR_ALREADYREGISTRED` | The client tried to send `USER` or `PASS` again after they have already successfully registered.                   |
| **464** | `ERR_PASSWDMISMATCH`   | The client failed to provide the correct server password via the `PASS` command.                                   |
