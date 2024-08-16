#undef UNICODE
#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUFSIZE 512



int main(int argc, TCHAR *argv[]) 
{ 
   HANDLE hPipe; 
   LPTSTR lpvMessage=TEXT("Mesaj de la client!"); 
   TCHAR chBuf[BUFSIZE]; 
   BOOL fSuccess; 
   DWORD cbRead, cbWritten, dwMode; 
   LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe"); 

   if( argc > 1 )
      lpvMessage = argv[1];
 
   // Incearca deschiderea pipe-ului cu nume;
   // asteapta daca este necesar
   int n;
   printf("Introduceti valoarea numarului n: ");
   scanf("%d", &n);
   while (1) 
   { 
      hPipe = CreateFile( 
         lpszPipename,   // nume pipe
         GENERIC_READ |  // acces read/write
         GENERIC_WRITE, 
         0,              // fara partajare
         NULL,           // atribute de securitate implicite
         OPEN_EXISTING,  // deschide un pipe existent
         0,              // atribute implicite
         NULL);          // fara fisier sablon/model (template)
 
      // Break daca pipe-ul este invalid
 
      if (hPipe != INVALID_HANDLE_VALUE) 
         break; 
 
      // Iesire daca apare o eroare alta decat ERROR_PIPE_BUSY
 
      if (GetLastError() != ERROR_PIPE_BUSY) 
      {
         printf("Nu pot deschide pipe!"); 
         return 0;
      }
 
      // Toate instantele pipe-ului sunt ocupate, asteptam 20 sec.
 
      if (!WaitNamedPipe(lpszPipename, 20000)) 
      { 
         printf("Timeout! Nu pot deschide pipe!"); 
         return 0;
      } 
   } 
 
   // Pipe-ul este conectat. Schimbare in mod de citire "mesaj"
 
   dwMode = PIPE_READMODE_MESSAGE; 
   fSuccess = SetNamedPipeHandleState( 
      hPipe,    // handle pipe
      &dwMode,  // noul mod pentru pipe
      NULL,     // nu seteaza nr, maxim bytes 
      NULL);    // nu seteaza timp maxim 
   if (!fSuccess) 
   {
      printf("Eroare la setarea pipe-ului!"); 
      return 0;
   }
 
   // Trimite un mesaj prin pipe server-ului
   for (int i = 0; i < n; i++) {
       char* message = (char*)malloc((i + 1) * sizeof(char));
       itoa(i, message, 10);
       printf("Voi trimite: %s\n", message);
       fSuccess = WriteFile(
           hPipe,
           message,
           (lstrlen(message) + 1) * sizeof(TCHAR),
           &cbWritten,
           NULL);
     

       fSuccess = ReadFile(
           hPipe,
           chBuf,
           BUFSIZE * sizeof(TCHAR),
           &cbRead,
           NULL);
  
       printf(TEXT("Am primit: %s\n"), chBuf);
   }

   char* message2 = "shutdown";
   fSuccess = WriteFile(
       hPipe,
       lpvMessage,
       (lstrlen(message2) + 1) * sizeof(TCHAR),
       &cbWritten,
       NULL);


   getch();
 
   CloseHandle(hPipe); 

   printf("Am trimis mesaj shutdown la server");
   return 0; 
}