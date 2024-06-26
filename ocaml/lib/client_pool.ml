module ClientPool = struct
  open Lwt.Infix
  
  module LRU = Lru.F.Make(String)
  
  type 'a t = {
    client_cache: (string, 'a) LRU.t Lwt_mutex.t;
  }
  
  let client_cache_size = 20
  
  module type Connectable = sig
    type t
    val connect : string -> t Lwt.t
  end
  
  let create () = 
    { client_cache = Lwt_mutex.create (LRU.empty client_cache_size) }
  
  let connect (type a) (module C : Connectable with type t = a) pool address =
    C.connect address >>= fun client ->
    Lwt_mutex.with_lock pool.client_cache (fun cache ->
      let cache = LRU.add address client cache in
      Lwt.return (client, { pool with client_cache = Lwt_mutex.create cache })
    )
  
  let get_client (type a) (module C : Connectable with type t = a) pool address =
    Lwt_mutex.with_lock pool.client_cache (fun cache ->
      match LRU.find address cache with
      | Some client -> Lwt.return (client, pool)
      | None -> 
          C.connect address >>= fun client ->
          let cache = LRU.add address client cache in
          Lwt.return (client, { pool with client_cache = Lwt_mutex.create cache })
    )
end