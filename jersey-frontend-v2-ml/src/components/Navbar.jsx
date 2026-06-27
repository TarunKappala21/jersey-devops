import { Link, useNavigate, useLocation } from 'react-router-dom'
import { ShoppingCart, LogOut, User, LayoutDashboard, Shirt } from 'lucide-react'
import { useAuth } from '../context/AuthContext'
import { useState, useEffect } from 'react'
import { getCart } from '../api/client'

export default function Navbar() {
  const { user, signOut } = useAuth()
  const navigate          = useNavigate()
  const location          = useLocation()
  const [cartCount, setCartCount] = useState(0)

  useEffect(() => {
    if (!user || user.role === 'admin') return
    const fanId = user.fanId || user.id || 101
    getCart(fanId)
      .then(r => setCartCount(r.data.data?.itemCount || 0))
      .catch(() => {})
  }, [user, location])

  const handleLogout = () => { signOut(); navigate('/login') }

  return (
    <nav style={{
      position: 'sticky', top: 0, zIndex: 100,
      background: 'rgba(10,10,10,0.92)',
      backdropFilter: 'blur(12px)',
      borderBottom: '1px solid var(--border)',
      padding: '0 24px',
    }}>
      <div className="container" style={{
        display: 'flex', alignItems: 'center',
        justifyContent: 'space-between', height: 60,
      }}>
        {/* Logo */}
        <Link to="/" style={{ textDecoration: 'none' }}>
          <span style={{
            fontFamily: 'var(--font-display)',
            fontSize: '1.6rem',
            color: 'var(--text-primary)',
            letterSpacing: '0.05em',
          }}>
            JERSEY<span style={{ color: 'var(--accent)' }}>VAULT</span>
          </span>
        </Link>

        {/* Nav links */}
        {user && (
          <div style={{ display: 'flex', gap: 4 }}>
            {[
              { to: '/',         label: 'Home' },
              { to: '/jerseys',  label: 'Shop' },
            ].map(({ to, label }) => (
              <Link key={to} to={to} style={{
                padding: '6px 14px',
                borderRadius: 'var(--radius-sm)',
                textDecoration: 'none',
                fontSize: '0.875rem',
                fontWeight: 500,
                color: location.pathname === to
                  ? 'var(--accent)' : 'var(--text-secondary)',
                background: location.pathname === to
                  ? 'var(--accent-glow)' : 'transparent',
                transition: 'var(--transition)',
              }}>{label}</Link>
            ))}
          </div>
        )}

        {/* Right side */}
        <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
          {user ? (
            <>
              <span style={{
                fontSize: '0.8rem',
                color: 'var(--text-secondary)',
                padding: '4px 10px',
                border: '1px solid var(--border)',
                borderRadius: 'var(--radius-sm)',
              }}>
                <span style={{ color: 'var(--accent)' }}>●</span> {user.name}
              </span>

              {user.role === 'admin' ? (
                <Link to="/admin" className="btn btn-ghost" style={{ padding: '6px 12px' }}>
                  <LayoutDashboard size={15} /> Dashboard
                </Link>
              ) : (
                <Link to="/cart" className="btn btn-ghost" style={{ padding: '6px 12px', position: 'relative' }}>
                  <ShoppingCart size={15} />
                  Cart
                  {cartCount > 0 && (
                    <span style={{
                      position: 'absolute', top: -6, right: -6,
                      background: 'var(--accent)', color: '#000',
                      fontSize: '0.65rem', fontWeight: 700,
                      width: 18, height: 18, borderRadius: '50%',
                      display: 'flex', alignItems: 'center', justifyContent: 'center',
                    }}>{cartCount}</span>
                  )}
                </Link>
              )}

              <button onClick={handleLogout} className="btn btn-ghost" style={{ padding: '6px 12px' }}>
                <LogOut size={15} />
              </button>
            </>
          ) : (
            <>
              <Link to="/login"    className="btn btn-ghost">Login</Link>
              <Link to="/register" className="btn btn-primary">Sign Up</Link>
            </>
          )}
        </div>
      </div>
    </nav>
  )
}
