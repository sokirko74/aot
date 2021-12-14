 server {
    server_name .aot.ru;
    #server_name 63.250.59.227;
    #server_name localhost;
    
    root /home/sokirko/RML/Source/www/wwwroot;
    index  index.html;
    client_max_body_size 5m;
    access_log /var/log/nginx/aot-access.log;
    error_log  /var/log/nginx/aot-error.log;



    location /cgi-bin/ { 
      # Disable gzip (it makes scripts feel slower since they have to complete
      # before getting gzipped)
      gzip off;

      # Set the root to /usr/lib (inside this location this means that we are
      # giving access to the files under /usr/lib/cgi-bin)
      root	/home/sokirko/RML/Source/www;

      # Fastcgi socket
      fastcgi_pass  unix:/var/run/fcgiwrap.socket;

      # Fastcgi parameters, include the standard ones
      include /etc/nginx/fastcgi_params;

    # Adjust non standard parameters (SCRIPT_FILENAME)
    #fastcgi_param SCRIPT_FILENAME  /usr/lib$fastcgi_script_name;
  }


    listen 80 ; 
}


