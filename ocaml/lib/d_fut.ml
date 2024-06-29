module DFut = struct
  type 'a t = {
    d_store_id: DStoreId.t;
  }

  let create d_store_id =
    { d_store_id }

  let share t =
    { d_store_id = DStoreId.clone t.d_store_id }

  let of_d_store_id d_store_id =
    create d_store_id
end