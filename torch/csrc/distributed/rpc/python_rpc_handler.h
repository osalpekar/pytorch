#pragma once

#include <torch/csrc/distributed/rpc/message.h>
#include <torch/csrc/utils/pybind.h>

namespace torch {
namespace distributed {
namespace rpc {

// Singleton class provides interface to execute python UDF remote call
// and deserialize the returned results by running python function
// in internal_rpc_utilities.
// The singleton object is constructed at first when RPC agent is
// constructed, where the python function in
// torch/distributed/internal_rpc_utils.py are imported only once.
class PYBIND11_EXPORT PythonRpcHandler {
 public:
  static PythonRpcHandler& getInstance();
  // Execute python UDF, result is pickled to binary string
  std::vector<char> generatePythonUDFResult(
      const std::vector<char>& pickledPayload,
      const std::vector<torch::Tensor>& requestTensorTable,
      std::vector<torch::Tensor>& responseTensorTable);
  // Returned python UDF result is pickled binary string, so run python
  // function to unpickle the python UDF result and return py::object to user
  py::object loadPythonUDFResult(
      const std::vector<char>& pickledPayload,
      const std::vector<torch::Tensor>& tensorTable);
  // Provide an interface to explicitly dec_ref runUDFFunction_ and
  // loadResultFunction_ before destructing PythonRpcHandler
  // In python 3.5, somehow when python program exits, before destructing
  // PythonRpcHandler, the python objects are cleaned up, then PythonRpcHandler
  // destructor will try to dec_ref deallocated objects and thus crashed
  // The solution is to explicitly clean up the python objects in
  // RpcAgent.join() before exiting program
  void cleanUp();

 private:
  PythonRpcHandler();
  ~PythonRpcHandler() = default;

  PythonRpcHandler(const PythonRpcHandler&) = delete;
  PythonRpcHandler& operator=(const PythonRpcHandler&) = delete;
  PythonRpcHandler(PythonRpcHandler&&) = delete;
  PythonRpcHandler& operator=(PythonRpcHandler&&) = delete;

  py::object runUDFFunction_;
  py::object loadResultFunction_;
};

} // namespace rpc
} // namespace distributed
} // namespace torch
