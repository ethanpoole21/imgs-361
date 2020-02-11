/** Implementation file for computing the DFT of a complex vector
 *
 *  \file ipcv/utils/Dft.cpp
 *  \author ethan poole erp1787
 *  \date 11/18/19
 */

#include <complex>
#include <vector>
#include <math.h>

#include "Dft.h"

using namespace std;
namespace ipcv {

std::vector<std::complex<double>> Dft(
    const std::vector<std::complex<double>>& f, int flag) {
  const std::complex<double> i(0, 1);

  int N = f.size();
  std::vector<std::complex<double>> F(N);

  double direction;
  complex<double> I = (0, -imag(i));

  if (flag == ipcv::DFT_INVERSE) {
    direction = 1;
    I = i;
  } else {
    direction = -1;
  }

  std::complex<double> scale(N, 0);

  for (double u = 0; u < N; u++) {
    std::complex<double> sum = (0.0, 0.0);
    for (double x = 0; x < N; x++) {
      std::complex<double> val(
          0.0, (2.0 * M_PI * u * x * (1 / static_cast<double>(N))));
      sum += f[x] * exp(val * I);
    }
    if (flag == ipcv::DFT_SCALE) {
      std::complex<double> comp_N(static_cast<double>(N), 0.0);

      F[u] = sum / comp_N;
    }

    else {
      F[u] = sum;
    }
  }

  return F;
}
}
