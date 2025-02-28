use std::{
    collections::{BTreeMap, BTreeSet, HashMap},
    ops::Range,
};

use evm::{
    backend::Log,
    executor::stack::{MemoryStackAccount, MemoryStackSubstate},
    ExitReason, Memory, Stack, Valids,
};
use primitive_types::{H160, H256, U256};

pub struct Continuation {
    pub data: Vec<u8>,
    pub code: Vec<u8>,
    pub position: Result<usize, ExitReason>,
    pub return_range: Range<U256>,
    pub valids: Valids,
    pub memory: Memory,
    pub stack: Stack,
    pub logs: Vec<Log>,
    pub accounts: BTreeMap<H160, MemoryStackAccount>,
    pub storages: BTreeMap<(H160, H256), H256>,
    pub deletes: BTreeSet<H160>,
}

pub struct Continuations {
    storage: HashMap<u64, Continuation>,
    next_continuation_id: u64,
}

impl Continuations {
    pub fn new() -> Self {
        Self {
            storage: HashMap::new(),
            next_continuation_id: 0,
        }
    }

    pub fn create_continuation(
        &mut self,
        machine: &mut evm::Machine,
        substate: &MemoryStackSubstate,
    ) -> u64 {
        self.next_continuation_id += 1;
        let continuation = Continuation {
            data: machine.data(),
            code: machine.code(),
            position: machine.position().to_owned(),
            return_range: machine.return_range().clone(),
            valids: machine.valids().clone(),
            memory: machine.memory().clone(),
            stack: machine.stack().clone(),
            accounts: substate.accounts().clone(),
            logs: Vec::from(substate.logs()),
            storages: substate.storages().clone(),
            deletes: substate.deletes().clone(),
        };
        self.storage.insert(self.next_continuation_id, continuation);
        self.next_continuation_id
    }

    pub fn get_contination(&mut self, id: u64) -> Option<Continuation> {
        let ret_val = self.storage.remove(&id);
        ret_val
    }
}
