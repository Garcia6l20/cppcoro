///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_NET_SOCKET_RECV_OPERATION_HPP_INCLUDED
#define CPPCORO_NET_SOCKET_RECV_OPERATION_HPP_INCLUDED

#include <cppcoro/config.hpp>
#include <cppcoro/cancellation_token.hpp>

#include <cstdint>

#if CPPCORO_OS_WINNT
# include <cppcoro/detail/win32.hpp>
# include <cppcoro/detail/win32_overlapped_operation.hpp>
#elif CPPCORO_OS_LINUX
# include <cppcoro/detail/linux_io_operation.hpp>
#endif

namespace cppcoro::net
{
	class socket;

	class socket_recv_operation_impl
	{
	public:

		socket_recv_operation_impl(
			socket& s,
			void* buffer,
			std::size_t byteCount) noexcept
			: m_socket(s)
			, m_buffer(buffer, byteCount)
		{}

		bool try_start(cppcoro::detail::io_operation_base& operation) noexcept;
		void cancel(cppcoro::detail::io_operation_base& operation) noexcept;

#if CPPCORO_OS_LINUX
		std::size_t get_result(cppcoro::detail::io_operation_base& operation);
#endif

	private:

		socket& m_socket;
		cppcoro::detail::sock_buf m_buffer;

	};

	class socket_recv_operation : public cppcoro::detail::io_operation<socket_recv_operation>
	{
	public:

		socket_recv_operation(
#if CPPCORO_OS_LINUX
			detail::lnx::io_queue& ioQueue,
#endif
			socket& s,
			void* buffer,
			std::size_t byteCount) noexcept
			: cppcoro::detail::io_operation<socket_recv_operation> {
#if CPPCORO_OS_LINUX
				  ioQueue
#endif
            }
			, m_impl(s, buffer, byteCount)
		{}

	private:

		friend cppcoro::detail::io_operation<socket_recv_operation>;

		bool try_start() noexcept { return m_impl.try_start(*this); }
#if CPPCORO_OS_LINUX
		std::size_t get_result() { return m_impl.get_result(*this); }
#endif

		socket_recv_operation_impl m_impl;

	};

	class socket_recv_operation_cancellable
		: public cppcoro::detail::io_operation_cancellable<socket_recv_operation_cancellable>
	{
	public:

		socket_recv_operation_cancellable(
#if CPPCORO_OS_LINUX
			detail::lnx::io_queue& ioQueue,
#endif
			socket& s,
			void* buffer,
			std::size_t byteCount,
			cancellation_token&& ct) noexcept
			: cppcoro::detail::io_operation_cancellable<socket_recv_operation_cancellable> {
#if CPPCORO_OS_LINUX
				  ioQueue,
#endif
				  std::move(ct)}
			, m_impl(s, buffer, byteCount)
		{}

	private:

		friend cppcoro::detail::io_operation_cancellable<socket_recv_operation_cancellable>;

		bool try_start() noexcept { return m_impl.try_start(*this); }
		void cancel() noexcept { m_impl.cancel(*this); }

		socket_recv_operation_impl m_impl;

	};

}  // namespace cppcoro::net

#endif
